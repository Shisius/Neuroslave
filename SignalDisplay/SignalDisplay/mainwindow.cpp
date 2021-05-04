#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    resize(500,500);
    d_centralWidget = new QWidget;
    d_signalPlot = new QCustomPlot;
    lbl_connection = new QLabel(tr("Нет соединения"));
    lbl_connection->adjustSize();
    lbl_connection->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(lbl_connection);
    layout->addWidget(d_signalPlot);
    d_centralWidget->setLayout(layout);
    setCentralWidget(d_centralWidget);

    createMenus();
    readSettings();
    initConnection();

    displaySignalSettings();
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

void MainWindow::initConnection()
{
    qDebug() << "initConnection";

    lbl_connection->setStyleSheet("color: red");

    d_tcpSocket = new QTcpSocket(this);

    connect(d_tcpSocket, SIGNAL(readyRead()), SLOT(slot_readTCP()));
    connect(d_tcpSocket, SIGNAL(error(QAbstractSocket::SocketError)),  SLOT(slot_catchErrorTCP(QAbstractSocket::SocketError)));
    connect(d_tcpSocket, SIGNAL(connected()), SLOT(slot_tcpConnected()));
    connect(d_tcpSocket, SIGNAL(disconnected()), SLOT(slot_tcpDisconnected()));
}

void MainWindow::slot_connection()
{
    qDebug() << "slot_connection()";
    if(dialog_connectionSettings == nullptr)
    {
       dialog_connectionSettings = new DialogConnectionSettings(this);
       dialog_connectionSettings->setIp(d_serverIP);
       dialog_connectionSettings->setPort(d_port);
    }
    if(dialog_connectionSettings->exec())
    {
        if(progbar_connecting == nullptr)
        {
            progbar_connecting = new QProgressBar(d_centralWidget);
            progbar_connecting->setGeometry(lbl_connection->geometry());
            progbar_connecting->setMaximum(0);
            progbar_connecting->setMinimum(0);
            progbar_connecting->hide();
        }

        if(d_serverIP != dialog_connectionSettings->serverIP())
        {
            d_serverIP = dialog_connectionSettings->serverIP();
            d_port = dialog_connectionSettings->serverPort();

            if(d_tcpSocket->state() == QAbstractSocket::ConnectedState)
                closeConnection();
        }
        else
        {
            if(d_port != dialog_connectionSettings->serverPort())
            {
                d_port = dialog_connectionSettings->serverPort();
                if(d_tcpSocket->state() == QAbstractSocket::ConnectedState)
                    closeConnection();
            }
        }
        tcpConnect();
    }
}

void MainWindow::tcpConnect()
{
    qDebug() << "tcpConnect";

    //if socket is not connected, the message should be written and then connection should occur
    if(d_tcpSocket->state() != QAbstractSocket::ConnectedState){
        progbar_connecting->show();
        lbl_connection->setText(tr("Соединение..."));
        lbl_connection->raise();
        lbl_connection->adjustSize();
        lbl_connection->setStyleSheet("color: black");
        d_tcpSocket->connectToHost(d_serverIP, d_port);
    }
}

void MainWindow::slot_catchErrorTCP(QAbstractSocket::SocketError)
{
    qDebug() << "slot_catchErrorTCP";
    QString error = d_tcpSocket->errorString();
    QString msgBoxName = d_serverIP + ": " + QString::number(d_port);
    QMessageBox::information(this, tr(msgBoxName.toStdString().c_str()), tr(error.toStdString().c_str()));

    statusBar()->showMessage(tr(((msgBoxName + ". Connection error.").toStdString()).c_str()), 2000);
    lbl_connection->setText(tr("Connection error."));
    lbl_connection->setStyleSheet("color: red");
    lbl_connection->adjustSize();
    progbar_connecting->hide();
}

void MainWindow::slot_tcpConnected()
{
    qDebug() << "slot_tcpConnected";
    QString msgBoxName = d_serverIP + ": " + QString::number(d_port);
    statusBar()->showMessage(tr(((msgBoxName + ". Connection established.").toStdString()).c_str()), 2000);
    lbl_connection->setText(tr("Connection established."));
    lbl_connection->setStyleSheet("color: green");
    lbl_connection->adjustSize();
    progbar_connecting->hide();
}

void MainWindow::slot_tcpDisconnected()
{
    qDebug()<<"slot_tcpDisconnected_msg()";
    QString error = d_tcpSocket->errorString();
    QString msgBoxName = d_serverIP + ": " + QString::number(d_port);
    QMessageBox::information(this, tr(msgBoxName.toStdString().c_str()), tr(error.toStdString().c_str()));

    statusBar()->showMessage(tr(((msgBoxName + ". Connection error.").toStdString()).c_str()), 2000);
    lbl_connection->setText(tr("Connection error."));
    lbl_connection->setStyleSheet("color: red");
    lbl_connection->adjustSize();
    progbar_connecting->hide();
}

void MainWindow::slot_readTCP()
{
    qDebug()<<"slot_readTCP";
    qDebug() << "d_tcpSocket_signal->bytesAvailable()" << d_tcpSocket->bytesAvailable();

    QDataStream in(d_tcpSocket);

    int restBlockSize = d_tcpSocket->bytesAvailable();
    int readBytes;
    if(restBlockSize < 0){
        QString str;
        return;
    }
    while(d_tcpSocket->bytesAvailable()>0) //
    {
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

void MainWindow::closeConnection()
{
    qDebug()<<"closeConnection";
    d_tcpSocket->flush();
    d_tcpSocket->close();
}

void MainWindow::displaySignalSettings()
{
    qDebug()<<"displaySignalSettings()";
    d_signalPlot->clearGraphs();
    iXSignal = 0;
    d_signalPlot->addGraph();
    d_signalPlot->graph(0)->setPen(QPen(Qt::blue)); // line color blue for first graph
    //d_signalPlot->graph(0)->setBrush(QBrush(QColor(0, 0, 255, 20))); // first graph will be filled with translucent blue

    // configure right and top axis to show ticks but no labels:
    // (see QCPAxisRect::setupFullAxesBox for a quicker method to do this)
    d_signalPlot->xAxis2->setVisible(true);
    d_signalPlot->xAxis2->setTickLabels(false);
    d_signalPlot->yAxis2->setVisible(true);
    d_signalPlot->yAxis2->setTickLabels(false);
    // make left and bottom axes always transfer their ranges to right and top axes:
    connect(d_signalPlot->xAxis, SIGNAL(rangeChanged(QCPRange)), d_signalPlot->xAxis2, SLOT(setRange(QCPRange)));
    connect(d_signalPlot->yAxis, SIGNAL(rangeChanged(QCPRange)), d_signalPlot->yAxis2, SLOT(setRange(QCPRange)));

    // Allow user to drag axis ranges with mouse, zoom with mouse wheel and select graphs by clicking:
    d_signalPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);
}

void MainWindow::addSignalToPlot(const std::vector<signalSample_t> &std_signal)
{

   // QVector<double> x(251), y0(251), y1(251);
    std::vector<double> vec_double(std_signal.begin(),std_signal.end());
    //QVector<double> addingSignal(sig.begin(),sig.end());
    QVector<double> qtSignal = QVector<double>::fromStdVector(vec_double);
    //d_signal.append(addingSignal);
    int sig_size = qtSignal.size();
    QVector<double> x(sig_size);

     for (int i = 0; i < sig_size; i++) {
         //qDebug() << x.at(i);
         //qDebug() << qtSignal.at(i);
         x[i] = iXSignal++;
     }
    // pass data points to graphs:
    d_signalPlot->graph(0)->addData(x, qtSignal, true);
    d_signalPlot->graph(0)->rescaleAxes();
    //Note: we could have also just called d_signalPlot->rescaleAxes(); instead
    d_signalPlot->replot();
}

void MainWindow::writeSettings()
{
    qDebug() << "writeSettings()";
    QSettings settings("Neuroslave", "EEG_GUI");
    settings.beginGroup("serverAddress");
    settings.setValue("IP", d_serverIP);
    settings.setValue("port", d_port);
    settings.endGroup();

    settings.beginGroup("mainwindow");
    settings.setValue("size", size());
    settings.setValue("state", saveState());
    settings.endGroup();
}

void MainWindow::readSettings()
{
    qDebug() << "readSettins";
    QSettings settings("Neuroslave", "EEG_GUI");
    d_serverIP = settings.value("serverAddress/IP","").toString();
    d_port = static_cast<quint16>(settings.value("serverAddress/port", 0).toUInt());

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

    closeConnection();
    event->accept();
}
