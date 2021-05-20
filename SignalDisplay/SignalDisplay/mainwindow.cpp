#include "mainwindow.h"
#include "stringlist_dialog.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    resize(500,500);
    d_centralWidget = new QWidget;
    d_signalPlot = new QCustomPlot;
    lbl_connection = new QLabel(tr("Нет соединения"));
    lbl_connection->adjustSize();
    lbl_connection->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);


    layout = new QVBoxLayout;
    layout->addWidget(lbl_connection);
    layout->addWidget(d_signalPlot);
    d_centralWidget->setLayout(layout);
    setCentralWidget(d_centralWidget);

    createMenus();
    createDocks();
    addToolBar(Qt::LeftToolBarArea, createToolBar());
    readSettings();
    initConnection();


//d_lastEegSession.n_channels = 4;
//    displaySignalSettings();

}

MainWindow::~MainWindow()
{
}

void MainWindow::createActions()
{
    qDebug() << "createActions";
    act_connection = new QAction(tr("Connect"), this);
    act_connection->setShortcut(tr("Alt+C"));
    connect(act_connection, SIGNAL(triggered()), SLOT(slot_connection()));
}

void MainWindow::createMenus()
{
    qDebug() << "createMenus";
    createActions();
    menu_settings = menuBar()->addMenu(tr("&Settings"));
    menu_settings->addAction(act_connection);
}

void MainWindow::createDocks()
{
    qDebug() << "createDocks()";
    QCommonStyle style;
    QAction *clearRadarMsgWindow = new QAction(style.standardIcon(QStyle::SP_LineEditClearButton), "Clear", this);
    connect(clearRadarMsgWindow, SIGNAL(triggered()), SLOT(slot_clearRadarMsgWindow()));
    //d_pdock_NeuroslaveMsg->addAction(clearRadarMsgWindow);
    d_ptb_NeuroslaveMsg = new QToolBar("Сообщения1");
    d_plbl_NeuroslaveMsg = new QLabel("Сообщения");
    d_ptb_NeuroslaveMsg->addWidget(d_plbl_NeuroslaveMsg);
    d_ptb_NeuroslaveMsg->addSeparator();
    d_ptb_NeuroslaveMsg->addAction(clearRadarMsgWindow);
    d_pdock_NeuroslaveMsg = new QDockWidget("Сообщения3", this);
    //d_pdock_NeuroslaveMsg->addAction(clearRadarMsgWindow);
    d_pdock_NeuroslaveMsg->setTitleBarWidget(d_ptb_NeuroslaveMsg);
    d_pte_NeuroslaveMsg = new QTextEdit(d_pdock_NeuroslaveMsg);
    d_pdock_NeuroslaveMsg->setWidget(d_pte_NeuroslaveMsg);
    addDockWidget(Qt::BottomDockWidgetArea, d_pdock_NeuroslaveMsg);
}

QToolBar* MainWindow::createToolBar()
{
    QToolBar* ptb = new QToolBar("Панель инструментов");
    QCommonStyle style;
    act_start_stop = ptb->addAction(style.standardIcon(QStyle::SP_MediaPlay), "Start", this, SLOT(slot_start()));
    ptb->addAction(style.standardIcon(QStyle::SP_DirIcon), "Playlist", this, SLOT(slot_chooseMusic()));
    ptb->addAction(style.standardIcon(QStyle::SP_DialogSaveButton), "Set", this, SLOT(slot_set()));
    return ptb;
}

void MainWindow::sendCommand(const QString & cmd)
{
    //QString str_sendCmd;
    //str_sendCmd = tr("%1%2%3%4%5%6").arg(cmd).arg(message_delimiter).arg(parameter).arg(message_delimiter).arg(value).arg(message_ending);
    qDebug() << "cmd " << cmd;
    QByteArray arrBlock(cmd.toUtf8());
    //arrBlock.append(str_sendCmd);

    qDebug() << arrBlock.data();
    QDataStream out;
    out.writeRawData(arrBlock.data(), arrBlock.size());
    d_tcpSocket_msg->write(arrBlock);
}

void MainWindow::slot_start()
{
    if(d_sessionStarted){
        stop();
    } else {
        sendCommand("Start"+message_ending);
    }
}

void MainWindow::start()
{
    d_sessionStarted = true;
    QCommonStyle style;
    act_start_stop->setIcon(style.standardIcon(QStyle::SP_MediaStop));
    act_start_stop->setText("Stop");
}

void MainWindow::stop()
{
    sendCommand("Stop"+message_ending);
    d_sessionStarted = false;
    QCommonStyle style;
    act_start_stop->setIcon(style.standardIcon(QStyle::SP_MediaPlay));
    act_start_stop->setText("Start");
}

void MainWindow::slot_set()
{
    QString str_sendCmd;
    QString str_sarStruct = QString::fromStdString(radar::to_json(d_lastEegSession));
    sarStructSettingsDialog dialog_sarStructSettings(str_sarStruct, this);
    if(dialog_sarStructSettings.exec())
    {
        QString eegSession_str = dialog_sarStructSettings.changedSarStructString();
        EegSession eegSession;
        if(radar::from_json(eegSession_str.toStdString(), eegSession))
        {
            str_sendCmd = tr("%1%2%3%4").arg("Set").arg(message_delimiter).arg(eegSession_str).arg(message_ending);
            //qDebug() << "str_sendCmd " << str_sendCmd;
            sendCommand(str_sendCmd);
        }
        else
        {
            QString error = "An error occured while settings.";
            QMessageBox::information(this, tr("Settings"), tr(error.toStdString().c_str()));
        }
    }

}

void MainWindow::slot_clearRadarMsgWindow()
{
    qDebug() << "slot_clearRadarMsgWindow()";
    d_pte_NeuroslaveMsg->clear();
}

void MainWindow::slot_chooseMusic()
{
    sendCommand("Choose"+message_ending);
//    QStringList playlist = {"1","2"};
//    StringList_Dialog playListDialog(playlist);
//    if(playListDialog.exec())
//    {
//        qDebug() << "fileName" << playListDialog.fileName();
//    }
    //processMessage("Playlist:[\"Yesterday.wav\", \"Imagine.mp3\", \"Yellow submarine.ogg\"]\n\r");
}

void MainWindow::initConnection()
{
    qDebug() << "initConnection";

    lbl_connection->setStyleSheet("color: red");

    lbl_port_msg = new QLabel();
    lbl_port_signal = new QLabel();
    lbl_port_msg->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    lbl_port_signal->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    lbl_port_msg->hide();
    lbl_port_signal->hide();

    layout->insertWidget(1, lbl_port_msg);
    layout->insertWidget(1, lbl_port_signal);

    //lbl_port_msg->setGeometry(ui->lbl_connection->geometry());
    //lbl_port_signal->setGeometry(ui->lbl_connection->geometry());

    //lbl_port_msg->move(lbl_port_msg->x(), lbl_port_signal->y()-2*lbl_port_frame->height());

    //connect(ui->pb_connection, SIGNAL(clicked()), SLOT(slot_connection()));
    //connect(ui->pb_saveImage, SIGNAL(clicked()), SLOT(slot_saveImage()));

    d_tcpSocket_msg = new QTcpSocket(this);
    d_tcpSocket_signal = new QTcpSocket(this);

    connect(d_tcpSocket_msg, SIGNAL(readyRead()), SLOT(slot_readTCP_msg()));
    connect(d_tcpSocket_msg, SIGNAL(error(QAbstractSocket::SocketError)),  SLOT(slot_catchErrorTCP_msg(QAbstractSocket::SocketError)));
    connect(d_tcpSocket_msg, SIGNAL(connected()), SLOT(slot_tcpConnected_msg()));
    connect(d_tcpSocket_msg, SIGNAL(disconnected()), SLOT(slot_tcpDisconnected_msg()));

    connect(d_tcpSocket_signal, SIGNAL(readyRead()), SLOT(slot_readTCP_signal()));
    connect(d_tcpSocket_signal, SIGNAL(error(QAbstractSocket::SocketError)),  SLOT(slot_catchErrorTCP_signal(QAbstractSocket::SocketError)));
    connect(d_tcpSocket_signal, SIGNAL(connected()), SLOT(slot_tcpConnected_signal()));
    connect(d_tcpSocket_signal, SIGNAL(disconnected()), SLOT(slot_tcpDisconnected_signal()));
}

void MainWindow::slot_connection()
{
    qDebug() << "slot_connection()";

    if(dialog_connectionSettings == nullptr)
    {
       dialog_connectionSettings = new DialogConnectionSettings(this);
       dialog_connectionSettings->setIp(d_serverIP);
       dialog_connectionSettings->setPort_msg(d_port_msg);
       dialog_connectionSettings->setPort_signal(d_port_signal);
    }
    if(dialog_connectionSettings->exec())
    {
        if(progbar_connecting_msg == nullptr)
        {
            progbar_connecting_msg = new QProgressBar(d_centralWidget);
            progbar_connecting_msg->setGeometry(lbl_connection->geometry());
            progbar_connecting_msg->setMaximum(0);
            progbar_connecting_msg->setMinimum(0);
            progbar_connecting_msg->hide();
        }
        if(progbar_connecting_signal == nullptr)
        {
            progbar_connecting_signal = new QProgressBar(d_centralWidget);
            progbar_connecting_signal->setGeometry(lbl_connection->geometry());
            progbar_connecting_signal->setMaximum(0);
            progbar_connecting_signal->setMinimum(0);
            progbar_connecting_signal->hide();
        }
        if(d_serverIP != dialog_connectionSettings->serverIP())
        {
            d_serverIP = dialog_connectionSettings->serverIP();
            d_port_msg = dialog_connectionSettings->serverPort_msg();
            d_port_signal= dialog_connectionSettings->serverPort_signal();
            if(d_tcpSocket_msg->state() == QAbstractSocket::ConnectedState)
                closeConnection_msg();
            if(d_tcpSocket_signal->state() == QAbstractSocket::ConnectedState)
                closeConnection_signal();
        }
        else
        {
            if(d_port_msg != dialog_connectionSettings->serverPort_msg())
            {
                d_port_msg = dialog_connectionSettings->serverPort_msg();
                if(d_tcpSocket_msg->state() == QAbstractSocket::ConnectedState)
                    closeConnection_msg();
            }
            if(d_port_signal != dialog_connectionSettings->serverPort_signal())
            {
                d_port_signal = dialog_connectionSettings->serverPort_signal();
                if(d_tcpSocket_signal->state() == QAbstractSocket::ConnectedState)
                    closeConnection_signal();
            }
        }
        tcpConnect();
    }
}

void MainWindow::tcpConnect()
{
    qDebug() << "tcpConnect";    

    //if one of the ports is already connected, the message should be written
    if(d_tcpSocket_msg->state() != QAbstractSocket::ConnectedState || d_tcpSocket_signal->state() != QAbstractSocket::ConnectedState){
        progbar_connecting_msg->show();
        lbl_connection->setText(tr("Connection..."));
        lbl_connection->raise();
        lbl_connection->adjustSize();
        lbl_connection->setStyleSheet("color: black");
    }
    //only then connection should occur
    if(d_tcpSocket_msg->state() != QAbstractSocket::ConnectedState)
        d_tcpSocket_msg->connectToHost(d_serverIP, d_port_msg);

    if(d_tcpSocket_signal->state() != QAbstractSocket::ConnectedState)
        d_tcpSocket_signal->connectToHost(d_serverIP, d_port_signal);
}

void MainWindow::slot_catchErrorTCP_msg(QAbstractSocket::SocketError)
{
    qDebug() << "slot_catchErrorTCP_msg";
    QString error = d_tcpSocket_msg->errorString();
    QString msgBoxName = QString::number(d_port_msg) + " port";
    QMessageBox::information(this, tr(msgBoxName.toStdString().c_str()), tr(error.toStdString().c_str()));

    statusBar()->showMessage(tr(((msgBoxName + ". Connection error.").toStdString()).c_str()), 2000);
    lbl_port_msg->setText(tr(((msgBoxName + ": Connection error.").toStdString()).c_str()));
    tcpConnectedDisplay(tcpPort::MSG, false);
}

void MainWindow::slot_catchErrorTCP_signal(QAbstractSocket::SocketError /*socketError*/)
{
    qDebug() << "slot_catchErrorTCP_signal";
    QString error = d_tcpSocket_signal->errorString();
    QString msgBoxName = QString::number(d_port_signal) + " port";
    QMessageBox::information(this, tr(msgBoxName.toStdString().c_str()), tr(error.toStdString().c_str()));
    statusBar()->showMessage(tr(((msgBoxName + ". Connection error.").toStdString()).c_str()), 2000);
    lbl_port_signal->setText(tr(((msgBoxName + ": Connection error.").toStdString()).c_str()));
    tcpConnectedDisplay(tcpPort::SIGNAL, false);
}

//void MainWindow::slot_tcpConnected()
//{
//    qDebug() << "slot_tcpConnected";
//    QString msgBoxName = d_serverIP + ": " + QString::number(d_port);
//    statusBar()->showMessage(tr(((msgBoxName + ". Connection established.").toStdString()).c_str()), 2000);
//    lbl_connection->setText(tr("Connection established."));
//    lbl_connection->setStyleSheet("color: green");
//    lbl_connection->adjustSize();
//    progbar_connecting->hide();
//}
void MainWindow::slot_tcpConnected_msg()
{
    qDebug() << "slot_tcpConnected_msg";
    tcpConnectedDisplay(tcpPort::MSG, true);
}

void MainWindow::slot_tcpConnected_signal()
{
    qDebug() << "slot_tcpConnected_signal";
    tcpConnectedDisplay(tcpPort::SIGNAL, true);
}

void MainWindow::slot_tcpDisconnected_msg()
{
    qDebug()<<"slot_tcpDisconnected_msg()";
    lbl_port_msg->setText(tr(((QString::number(d_port_msg) + " port: connection was lost.").toStdString()).c_str()));
    statusBar()->showMessage(tr(((QString::number(d_port_msg) + " port: connection was lost.").toStdString()).c_str()), 2000);
    tcpConnectedDisplay(tcpPort::MSG, false);
}

void MainWindow::slot_tcpDisconnected_signal()
{
    qDebug()<<"slot_tcpDisconnected_signal()";
    lbl_port_signal->setText(tr(((QString::number(d_port_signal) + " port: connection was lost.").toStdString()).c_str()));
    statusBar()->showMessage(tr(((QString::number(d_port_signal) + " port: connection was lost.").toStdString()).c_str()), 2000);
    tcpConnectedDisplay(tcpPort::SIGNAL, false);
}

//void MainWindow::slot_tcpDisconnected()
//{
//    qDebug()<<"slot_tcpDisconnected_msg()";
//    QString error = d_tcpSocket->errorString();
//    QString msgBoxName = d_serverIP + ": " + QString::number(d_port);
//    QMessageBox::information(this, tr(msgBoxName.toStdString().c_str()), tr(error.toStdString().c_str()));

//    statusBar()->showMessage(tr(((msgBoxName + ". Connection error.").toStdString()).c_str()), 2000);
//    lbl_connection->setText(tr("Connection error."));
//    lbl_connection->setStyleSheet("color: red");
//    lbl_connection->adjustSize();
//    progbar_connecting->hide();
//}

void MainWindow::tcpConnectedDisplay(tcpPort port, bool isConnected)
{
    lbl_connection->hide();
    progbar_connecting_msg->hide();//hide it as common progbar_connecting
    lbl_port_msg->show();
    lbl_port_msg->raise();
    lbl_port_signal->show();
    lbl_port_signal->raise();

    //QString connectedStringRus =  " порт: соединение yстановлено.";
    QString connectedString =  " port: connection was established.";

    switch (port) {
    case tcpPort::MSG:
        if(isConnected)
        {
            lbl_port_msg->setText(tr(((QString::number(d_port_msg) + connectedString).toStdString()).c_str()));
            lbl_port_msg->setStyleSheet("color: green");
            statusBar()->showMessage(tr(((QString::number(d_port_msg) + connectedString).toStdString()).c_str()), 2000);

        }
        else
        {
            lbl_port_msg->setStyleSheet("color: red");
        }
        lbl_port_msg->adjustSize();
        progbar_connecting_msg->hide();

        if(d_tcpSocket_signal->state() == QAbstractSocket::ConnectingState){
            lbl_port_signal->setText(tr("Connection.."));
            lbl_port_signal->adjustSize();
            lbl_port_signal->setStyleSheet("color: black");
            progbar_connecting_signal->show();
            return;
        }
        break;

    case tcpPort::SIGNAL:
        if(isConnected)
        {
            lbl_port_signal->setText(tr(((QString::number(d_port_signal) + connectedString).toStdString()).c_str()));
            lbl_port_signal->setStyleSheet("color: green");
            statusBar()->showMessage(tr(((QString::number(d_port_signal) + connectedString).toStdString()).c_str()), 2000);
        }
        else
        {
            lbl_port_signal->setStyleSheet("color: red");
        }
        lbl_port_signal->adjustSize();
        progbar_connecting_signal->hide();
        if(d_tcpSocket_msg->state() == QAbstractSocket::ConnectingState){
            lbl_port_msg->setText(tr("Connection.."));
            lbl_port_msg->adjustSize();
            lbl_port_msg->setStyleSheet("color: black");
            progbar_connecting_msg->show();
            return;
        }
        break;
    default:
        break;
    }
}

void MainWindow::slot_readTCP_msg()
{
    qDebug()<<"slot_readTCP_msg";
    qDebug() << "d_tcpSocket_msg->bytesAvailable()" << d_tcpSocket_msg->bytesAvailable();
    //writeErrorLog(QString::);
    QTextStream txtIn(d_tcpSocket_msg);

    int bytesAvailable = d_tcpSocket_msg->bytesAvailable();

    while(bytesAvailable > 0)
    {
        bytesAvailable--;
        char ch;
        txtIn >> ch;
        //qDebug()<<"ch" << ch;
        d_textMessage.append(ch);
        if (d_textMessage.endsWith(message_ending))
        {
            //writeTcpLog_msg("[Radar message] " + d_radarMsgString);
            qDebug()<<d_textMessage;
            processMessage(d_textMessage);
            d_textMessage.clear();
        }
        //qDebug() << "d_tcpSocket_msg->bytesAvailable()" << d_tcpSocket_msg->bytesAvailable();
    }
    qDebug()<<"slot_readTCP_msg_end";
}

void MainWindow::processMessage(const QString &msg)
{
    qDebug()<<"processMessage";
    qDebug()<<msg;
    QString addedText;
    //EegSession eegSession;
    //std::string eegSession_str = radar::get_type_name(eegSession);
    QString eegSessionName_str = QString::fromStdString(radar::get_type_name(d_lastEegSession));

    if(msg.startsWith(eegSessionName_str)){
        addedText = msg;
        std::string msg_std_str = msg.toStdString();
        if(radar::from_json(msg_std_str, d_lastEegSession))
        {
            displaySignalSettings();
            start();
        }
    }
    if(msg.startsWith(textMessage_beginning)){
        addedText = msg;
    }
    if(msg.startsWith(textError_beginning)){
        addedText = QString("<span style=\" color:#ff0000;\">%1</span>").arg(msg);
    }
    if(msg.startsWith(textWarning_beginning)){
        addedText = QString("<span style=\" color:#ff8c00;\">%1</span>").arg(msg);
    }
    if(msg.startsWith(msgPlaylist_beginning)){
        addedText = msg;
        qDebug() << addedText;
        //QString rem_str = msgPlaylist_beginning + QString(message_delimiter);
        //int rem_str_count = rem_str.count();
        QString jsonPlaylist_str = msg;
        int ind_message_delimiter = msg.indexOf(message_delimiter);
        jsonPlaylist_str.remove(0, ind_message_delimiter+1);
        jsonPlaylist_str.remove(message_ending);
        qDebug() << jsonPlaylist_str;
        QJsonDocument jsonPlaylist = QJsonDocument::fromJson(jsonPlaylist_str.toUtf8());
        if(jsonPlaylist.isNull())
            return;
        if(jsonPlaylist.isArray())
        {
            QJsonArray jsonArray = jsonPlaylist.array();
            QVariantList playlist_qvariant = jsonArray.toVariantList();
            QStringList playlist_stringList;
            foreach(QVariant track, playlist_qvariant)
            {
                playlist_stringList << track.toString();
            }
            StringList_Dialog playListDialog(playlist_stringList);
            if(playListDialog.exec())
            {
                QString fileName = playListDialog.fileName();
                qDebug() << "fileName" << fileName;

                sendCommand("Choose"+QString(message_delimiter)+fileName+message_ending);
            }

        }
    }
    d_pte_NeuroslaveMsg->append(addedText);
}

void MainWindow::slot_readTCP_signal()
{
    qDebug()<<"slot_readTCP_signal";
    if(!d_sessionStarted)
        return;
    qDebug() << "d_tcpSocket_signal->bytesAvailable()" << d_tcpSocket_signal->bytesAvailable();

    QDataStream in(d_tcpSocket_signal);

    int bytesAvailable = d_tcpSocket_signal->bytesAvailable();

    while(bytesAvailable > 0)
    {
        Header header;
        //uint32_t header;
        in >> header.Label;
        qDebug() << "header.Label"<< header.Label;
        bytesAvailable -= sizeof(header.Label);
        if(header.Label != NeuroslaveLabel)
            return;
        in >> header.payload_length;
        qDebug() << "header.payload_length"<< header.payload_length;
        bytesAvailable -= sizeof(header.payload_length);
        if(header.payload_length != d_lastEegSession.n_channels)
            return;
        in >> header.reserved;
        qDebug() << "header.reserved"<< header.reserved;
        bytesAvailable -= sizeof(header.reserved);
        if(bytesAvailable < header.payload_length)
            return;
        //QVector<int32_t> points(header.payload_length);
        for(uint8_t i = 0; i < header.payload_length; i++)
        {
            int32_t point;
            in >> point;
            d_points[i].push_back(static_cast<double>(point));
            qDebug() << "point"<< point;
            bytesAvailable -= sizeof(point);
        }
        if(d_points.at(0).size() == DECIMATION_KOEFF){
            QVector<double> x(DECIMATION_KOEFF);
            for(uint i = 0; i < DECIMATION_KOEFF; i++)
            {
                x[i] = iXSignal*DECIMATION_KOEFF + i;
            }
            addData(x, d_points);
            d_points.clear();
            d_points.resize(d_lastEegSession.n_channels);

            for (uint i = 0; i < d_lastEegSession.n_channels; i++) {
                d_points[i].reserve(DECIMATION_KOEFF);
            }
            iXSignal++;
        }
//        if(!d_isSarSessionRecieved){ // if sarSession struct hasn't received yet
//            readBytes = 0;
//            char ch[1]; //for received char
//            do
//            {
//                in.readRawData(ch,1); //receive 1 char
//                //qDebug()<<"ch" << ch;
//                d_sarSessionStructJson += ch[0]; //add received char to string
//                d_tcpOutStream_signal.writeRawData(ch,1); //write received char to signal binary log
//                readBytes++; // increase read bytes
//            }while(ch[0] != radar::message_ending && d_tcpSocket_frame->bytesAvailable()>0); //until radar::message_ending

//            qDebug()<<"sarSessionStructJson.size()" << d_sarSessionStructJson.size();
//            qDebug()<<"readBytes (for sarSessionStructJson): "<< readBytes;
//            writeTcpLog_msg("[Radar signal] " + QString::fromStdString(d_sarSessionStructJson)); //write received string with sarSession struct json string to TCP log

//            size_t ind = d_sarSessionStructJson.find(radar::message_delimiter);
//            if(ind == std::string::npos)
//                return;
//            d_sarSessionStructJson.erase(0, ind+1); // remove chars before sarSession struct json string

//            qDebug()<<"sarSessionStructJson" << QString::fromStdString(d_sarSessionStructJson);
//            radar::from_json(d_sarSessionStructJson, d_sarSessionForSignal); // get sarSession struct

//            d_isSarSessionRecieved = true; //we recieved sarSession struct
//            d_sarSessionStructJson.clear(); //clear string for the next sarSession

//            if (!d_signalBuffer.isOpen())
//                d_signalBuffer.open(QBuffer::WriteOnly); //open buffer for signal data

//            displaySignalSettings();

//            if(restBlockSize == readBytes){ //if we recieved only sarSession struct then leave
//                break;
//                //return;
//            }
//            restBlockSize -= readBytes; //otherwise calc how many bytes still available
//        }

//        qDebug()<<"sarSession.adc_sample_buf_len: "<<d_sarSessionForSignal.adc_sample_buf_len;

//        if(restBlockSize > static_cast<int>(d_sarSessionForSignal.adc_sample_buf_len) - d_receivedSignalBlockSize){ //if we received more than the residue of the signal
//            restBlockSize = d_sarSessionForSignal.adc_sample_buf_len - d_receivedSignalBlockSize; //we will read only the residue of the signal
//        }
//        qDebug() << "d_tcpSocket_signal->bytesAvailable()" << d_tcpSocket_signal->bytesAvailable();
//        qDebug() << "restBlockSize after received sarSessionStructJson" << restBlockSize;
//        const int signalSize = restBlockSize;
//        char signal[signalSize];
//        readBytes = in.readRawData(signal, signalSize); //read part of the signal
//        qDebug() << "readBytes (for signal part)" << readBytes;
//        d_signalBuffer.write(signal,readBytes); //write part of the signal to the signal buffer
//        d_tcpOutStream_signal.writeRawData(signal,readBytes);  //write received part of the signal to signal binary log
//        qDebug() << "d_signalBuffer.size()" << d_signalBuffer.size();
//        d_receivedSignalBlockSize += readBytes; //add received signal part bytes to the previous received signal block

//        if(d_receivedSignalBlockSize == static_cast<int>(d_sarSessionForSignal.adc_sample_buf_len)){ //if we received the whole signal
//            std::vector<sar::adc_sample_t> signal_vector;
//            sar::parse_adc_pack(d_signalBuffer.buffer().constData(), d_sarSessionForSignal.adc_sample_buf_len, d_sarSessionForSignal.cpu_pp_parameters.ADC_N_recv, d_lastSarInitial.fpga_config.adc_header_length, signal_vector); //parse signal data to signal_vector
//            addSignalToPlot(signal_vector);
//            d_receivedSignalBlockSize = 0;
//            d_signalBuffer.seek(d_receivedSignalBlockSize);
//            d_signalBuffer.close();
//            if(!d_isSarSessionRecieved) //last signal part is received before in_process become false. So this code part doesn't work
//            {
//                //d_signal.clear();
//                d_tcpFile_signal.close();
//                d_tcpOutStream_signal.resetStatus();
//                qDebug() << "d_tcpFile_signal.close()";
//                d_isSarSessionRecieved = false; //will be waiting for a new sarSession struct
//            }
//            qDebug() << "d_tcpSocket_signal->bytesAvailable() after received signal" << d_tcpSocket_signal->bytesAvailable();
//            d_signalBuffer.open(QBuffer::WriteOnly); //open buffer for signal data
//        }
//        if (d_signalBuffer.isOpen())
//            d_signalBuffer.seek(d_receivedSignalBlockSize);
    }

}

void MainWindow::closeConnections()
{
    qDebug()<<"closeConnections";
    closeConnection_msg();
    closeConnection_signal();
}

void MainWindow::closeConnection_msg()
{
    qDebug()<<"closeConnection_msg";
    d_tcpSocket_msg->flush();
    d_tcpSocket_msg->close();
}
void MainWindow::closeConnection_signal()
{
    qDebug()<<"closeConnection_signal";
    d_tcpSocket_signal->flush();
    d_tcpSocket_signal->close();
}

void MainWindow::displaySignalSettings()
{
    qDebug()<<"displaySignalSettings()";
    //d_signalPlot->clearGraphs();

    d_signalPlot->plotLayout()->clear();
    d_graph.clear();
    d_graph.resize(d_lastEegSession.n_channels);

    d_points.clear();
    d_points.resize(d_lastEegSession.n_channels);

    for (uint i = 0; i < d_lastEegSession.n_channels; i++) {
        d_points[i].reserve(DECIMATION_KOEFF);
    }
    iXSignal = 0;
    //QList<QCPAxis*> allAxes;
    for(uint i = 0; i < d_lastEegSession.n_channels; i++)
    {
        QCPAxisRect* axisRect = new QCPAxisRect(d_signalPlot);
        d_signalPlot->plotLayout()->addElement(i,0,axisRect);

        d_allAxes << axisRect->axes();
        d_graph[i] = d_signalPlot->addGraph(axisRect->axis(QCPAxis::atBottom), axisRect->axis(QCPAxis::atLeft));
        if(d_graph[i] == nullptr)
            qDebug()<<"d_graph["<<i<<"] is nullptr";
        d_graph[i]->setPen(QPen(Qt::red,2));
//        for(int j = 0; j < 50; j++)
//        {
//            d_graph[i]->addData(j+i,j);
//            d_graph[i]->rescaleAxes();
//        }
        //d_signalPlot->addGraph();
        //d_signalPlot->graph(0)->setPen(QPen(Qt::blue)); // line color blue for first graph
    }
    foreach(QCPAxisRect *rect, d_signalPlot->axisRects())
    {
        foreach (QCPAxis *axis, rect->axes())
        {
          axis->setLayer("axes");
          axis->grid()->setLayer("grid");
        }
    }
    //d_signalPlot->graph(0)->setBrush(QBrush(QColor(0, 0, 255, 20))); // first graph will be filled with translucent blue

    // configure right and top axis to show ticks but no labels:
    // (see QCPAxisRect::setupFullAxesBox for a quicker method to do this)
//    d_signalPlot->xAxis2->setVisible(true);
//    d_signalPlot->xAxis2->setTickLabels(false);
//    d_signalPlot->yAxis2->setVisible(true);
//    d_signalPlot->yAxis2->setTickLabels(false);
//    // make left and bottom axes always transfer their ranges to right and top axes:
    //connect(d_signalPlot->xAxis, SIGNAL(rangeChanged(QCPRange)), d_signalPlot->xAxis2, SLOT(setRange(QCPRange)));
    //connect(d_signalPlot->yAxis, SIGNAL(rangeChanged(QCPRange)), d_signalPlot->yAxis2, SLOT(setRange(QCPRange)));

    // Allow user to drag axis ranges with mouse, zoom with mouse wheel and select graphs by clicking:
    d_signalPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);
    d_signalPlot->replot();
}

void MainWindow::addData(const QVector<double> &x, const QVector<QVector<double>> &addedPoints)
{
    qDebug()<<"addData";
    for (uint i = 0; i < d_lastEegSession.n_channels; i++) {        
        d_graph[i]->addData(x,addedPoints.at(i));
        //d_graph[i]->rescaleAxes();
        qDebug()<<"addedPoints.at("<<i<<")" << addedPoints.at(i);
        //qDebug()<<"iXSignal" << iXSignal;
    }
    if((iXSignal+1)*DECIMATION_KOEFF > signalSize){
        for (uint i = 0; i < d_lastEegSession.n_channels; i++) {
            QSharedPointer<QCPGraphDataContainer> graphData = d_graph.at(i)->data();
            graphData->removeBefore((iXSignal+1)*DECIMATION_KOEFF - signalSize);
            d_graph[i]->setData(graphData);
            //d_graph[i]->rescaleAxes();
        }
    }
    d_signalPlot->rescaleAxes();

//     for (int i = 0; i < sig_size; i++) {
//         //qDebug() << x.at(i);
//         //qDebug() << qtSignal.at(i);
//         x[i] = iXSignal++;
//     }
//    // pass data points to graphs:
//    d_signalPlot->graph(0)->addData(x, qtSignal, true);
//    d_signalPlot->graph(0)->rescaleAxes();
    //Note: we could have also just called d_signalPlot->rescaleAxes(); instead
    d_signalPlot->replot();
    //iXSignal++;
    qDebug()<<"addData_end";
}

void MainWindow::writeSettings()
{
    qDebug() << "writeSettings()";
    QSettings settings("Neuroslave", "EEG_GUI");
    settings.beginGroup("serverAddress");
    settings.setValue("IP", d_serverIP);
    settings.setValue("port_msg", d_port_msg);
    settings.setValue("port_signal", d_port_signal);
    settings.endGroup();

    settings.beginGroup("mainwindow");
    settings.setValue("size", size());
    settings.setValue("state", saveState());
    settings.endGroup();
}

void MainWindow::readSettings()
{
    qDebug() << "readSettings";
    QSettings settings("Neuroslave", "EEG_GUI");
    d_serverIP = settings.value("serverAddress/IP","").toString();
    d_port_msg = static_cast<quint16>(settings.value("serverAddress/port_msg", 0).toUInt());
    d_port_signal = static_cast<quint16>(settings.value("serverAddress/port_signal", 0).toUInt());

    settings.beginGroup("mainwindow");
    resize(settings.value("size").toSize());
    restoreState(settings.value("state").toByteArray());
    settings.endGroup();
}

void MainWindow::closeEvent(QCloseEvent* event)
{
    qDebug() << "closeEvent";
    int r = QMessageBox::warning(this, tr("Close GUI"), tr("Save Settings?"), QMessageBox::Yes | QMessageBox::Default, QMessageBox::No, QMessageBox::Cancel | QMessageBox::Escape);
    if(r == QMessageBox::Cancel){
        event->ignore();
        return;
    }
    else{
        if(r == QMessageBox::Yes)
            writeSettings();
    }

    closeConnections();
    event->accept();
}
