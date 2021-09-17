#include "mainwindow.h"
#include "stringlist_dialog.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    resize(1000,1000);
    d_centralWidget = new QWidget;
    d_signalPlot = new QCustomPlot;
    lbl_connection = new QLabel(tr("No connection"));
    lbl_connection->adjustSize();
    lbl_connection->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);


    d_centralLayout = new QVBoxLayout;
    d_centralLayout->addWidget(lbl_connection);
    d_centralLayout->addWidget(d_signalPlot);
    d_centralWidget->setLayout(d_centralLayout);
    setCentralWidget(d_centralWidget);

    createMenus();
    createDocks();
    addToolBar(Qt::LeftToolBarArea, createToolBar());
    readSettings();
    initConnection();
    qDebug() << qToLittleEndian(NeuroslaveLabel);
    qDebug() << qToBigEndian(NeuroslaveLabel);
    qDebug() << NeuroslaveLabel;


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
    d_ptb_NeuroslaveMsg = new QToolBar("Messages");
    d_plbl_NeuroslaveMsg = new QLabel("Messages");
    d_ptb_NeuroslaveMsg->addWidget(d_plbl_NeuroslaveMsg);
    d_ptb_NeuroslaveMsg->addSeparator();
    d_ptb_NeuroslaveMsg->addAction(clearRadarMsgWindow);
    d_pdock_NeuroslaveMsg = new QDockWidget(this);
    //d_pdock_NeuroslaveMsg->set
    //d_pdock_NeuroslaveMsg->addAction(clearRadarMsgWindow);
    d_pdock_NeuroslaveMsg->setTitleBarWidget(d_ptb_NeuroslaveMsg);
    d_pte_NeuroslaveMsg = new QTextEdit(d_pdock_NeuroslaveMsg);
    d_pdock_NeuroslaveMsg->setWidget(d_pte_NeuroslaveMsg);
    addDockWidget(Qt::BottomDockWidgetArea, d_pdock_NeuroslaveMsg);
}

QToolBar* MainWindow::createToolBar()
{
    qDebug() << "createToolBar()";
    QToolBar* ptb = new QToolBar();
    QCommonStyle style;
    act_on_off     = ptb->addAction(QIcon(":/img/turnon.png"), d_cmdStrings_map.value(TurnOn), this, SLOT(slot_start()));
    ptb->addSeparator();
    act_EegSessionSet        = ptb->addAction(QIcon(":/img/EegSettings.png"),"EegSession settings", this, SLOT(slot_EegSessionSet()));
    ptb->addSeparator();
    act_playlist   = ptb->addAction(QIcon(":/img/playlist.png"), "Playlist", this, SLOT(slot_chooseMusic()));
    ptb->addSeparator();
    act_record     = ptb->addAction(QIcon(":/img/play.png"), d_cmdStrings_map.value(Record), this, SLOT(slot_record()));
    ptb->addSeparator();
    act_gameSet    = ptb->addAction(QIcon(":/img/GameSettings.png"), d_cmdStrings_map.value(Game) + " settings", this, SLOT(slot_gameSet()));
    ptb->addSeparator();
    act_game       = ptb->addAction(QIcon(":/img/game.png"), d_cmdStrings_map.value(Game), this, SLOT(slot_game()));

    ptb->setIconSize(QSize(50,50));

    act_on_off->setDisabled(true);
    act_playlist  ->setDisabled(true);
    act_EegSessionSet->setDisabled(true);
    act_record    ->setDisabled(true);
    act_game->setDisabled(true);
    act_gameSet->setDisabled(true);
    return ptb;
}

void MainWindow::sendCommand(const QString & cmd)
{
    qDebug() << "sendCommand()";
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
    qDebug() << "slot_start()";
    if(d_sessionStarted){
        stop();
    } else {
        sendCommand(d_cmdStrings_map.value(TurnOn)+message_ending);
    }
}

void MainWindow::start()
{
    qDebug() << "start()";
    d_sessionStarted = true;
    //QCommonStyle style;
    act_on_off->setIcon(QIcon(":/img/turnoff.png"));
    act_on_off->setText(d_cmdStrings_map.value(TurnOff));
    act_record->setEnabled(true);
    act_game->setEnabled(true);
    act_gameSet->setEnabled(true);
    d_gameState = GameState::Finished;
    act_EegSessionSet->setDisabled(true);
}

void MainWindow::stop()
{
    qDebug() << "stop()";
    sendCommand(d_cmdStrings_map.value(TurnOff)+message_ending);
    d_sessionStarted = false;
    //QCommonStyle style;
    act_on_off->setIcon(QIcon(":/img/turnon.png"));
    act_on_off->setText(d_cmdStrings_map.value(TurnOn));
    act_record->setEnabled(false);
    act_EegSessionSet->setEnabled(true);
}

void MainWindow::slot_EegSessionSet()
{
    qDebug() << "slot_EegSessionSet()";
    QString str_sendCmd;

    QString str_EegSession = QString::fromStdString(d_lastEegSession.to_json());

    sarStructSettingsDialog dialog_sarStructSettings(str_EegSession, "EegSession settings", this);
    if(dialog_sarStructSettings.exec())
    {
        QString eegSession_str = dialog_sarStructSettings.changedSarStructString();
        NeuroslaveSession eegSession;
        if(eegSession.from_json(eegSession_str.toStdString())/*radar::from_json(eegSession_str.toStdString(), eegSession)*/)
        {
            str_sendCmd = tr("%1%2%3%4").arg(d_cmdStrings_map.value(Set)).arg(message_delimiter).arg(eegSession_str).arg(message_ending);
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

void MainWindow::slot_gameSet()
{
    qDebug() << "slot_gameSet()";
    QString str_sendCmd;
    NeuroslaveGameSettings gameSettings;

    //QString str_gameSettings_empty = QString::fromStdString(radar::to_json(gameSettings));
    QJsonObject gameSettings_empty_jo;
    gameSettings_empty_jo["subfolder"] = QString::fromStdString(gameSettings.subfolder);
    gameSettings_empty_jo["duration"] = gameSettings.duration;
    gameSettings_empty_jo["volume"] = gameSettings.volume;
    gameSettings_empty_jo["complexity"] = gameSettings.complexity;
    QJsonDocument gameSettings_empty_jd(gameSettings_empty_jo);
    QByteArray gameSettings_empty_ba = gameSettings_empty_jd.toJson(QJsonDocument::Compact);
    QString str_gameSettings_empty(gameSettings_empty_ba);
    sarStructSettingsDialog dialog_sarStructSettings(str_gameSettings_empty, "Game settings", this);
    if(dialog_sarStructSettings.exec())
    {
        QString gameSettings_str = dialog_sarStructSettings.changedSarStructString();
        QJsonDocument gameSettings_jd = QJsonDocument::fromJson(gameSettings_str.toUtf8());
        if(gameSettings_jd.isNull())
            return;
        if(gameSettings_jd.isObject())
        {
//            QJsonObject gameSettings_jo = gameSettings_jd.object();
//            if (gameSettings_jo.contains("subfolder") && gameSettings_jo["subfolder"].isString())
//                    gameSettings.subfolder = gameSettings_jo["subfolder"].toString().toStdString();
//            if (gameSettings_jo.contains("duration") && gameSettings_jo["duration"].isDouble())
//                    gameSettings.duration = gameSettings_jo["duration"].toDouble();
//            if (gameSettings_jo.contains("volume") && gameSettings_jo["volume"].isDouble())
//                    gameSettings.volume = gameSettings_jo["volume"].toDouble();
//            if (gameSettings_jo.contains("complexity") && gameSettings_jo["complexity"].isDouble())
//                    gameSettings.complexity = gameSettings_jo["complexity"].toInt();
            str_sendCmd = tr("%1%2%3%4").arg(d_cmdStrings_map.value(Set)).arg(message_delimiter).arg(gameSettings_str).arg(message_ending);
            qDebug() << "str_sendCmd " << str_sendCmd;
            sendCommand(str_sendCmd);
        }

//        if(radar::from_json(gameSettings_str.toStdString(), gameSettings))
//        {
//            str_sendCmd = tr("%1%2%3%4").arg(d_cmdStrings_map.value(Set)).arg(message_delimiter).arg(gameSettings_str).arg(message_ending);
//            //qDebug() << "str_sendCmd " << str_sendCmd;
//            sendCommand(str_sendCmd);
//        }
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
    qDebug() << "slot_chooseMusic()";
    sendCommand(d_cmdStrings_map.value(Choose)+message_ending);
}

void MainWindow::slot_record()
{
    qDebug() << "slot_record()";
    if(d_recordStarted)
    {
        //d_recordStarted = false;
        sendCommand(d_cmdStrings_map.value(Stop)+message_ending);
    } else {
        sendCommand(d_cmdStrings_map.value(Record)+message_ending);
//        QCommonStyle style;
//        act_record->setIcon(style.standardIcon(QStyle::SP_MediaPlay));
//        act_record->setText(d_cmdStrings_map.value(Record));
        //d_recordStarted = true;
    }
}

void MainWindow::slot_game()
{
    qDebug() << "slot_game()";
    sendCommand(d_cmdStrings_map.value(Game)+message_ending);
    d_gameState = GameState::Started;
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

    d_centralLayout->insertWidget(1, lbl_port_msg);
    d_centralLayout->insertWidget(1, lbl_port_signal);

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

//void MainWindow::initSignalReceiving()
//{

//}

void MainWindow::slot_connection()
{
    qDebug() << "slot_connection()";

    if(dialog_connectionSettings == nullptr)
    {
       dialog_connectionSettings = new DialogConnectionSettings(this);
       dialog_connectionSettings->setIp(d_serverIP);
       dialog_connectionSettings->setPort_msg(d_port_msg);
       dialog_connectionSettings->setPort_signal(d_port_signal);
       dialog_connectionSettings->setLogin(d_login);
       dialog_connectionSettings->setPassword(d_password);
    }
    if(dialog_connectionSettings->exec())
    {
        if(d_login != dialog_connectionSettings->login())
        {
            d_login = dialog_connectionSettings->login();
        }
        if(d_password != dialog_connectionSettings->password())
        {
            d_password = dialog_connectionSettings->password();
        }

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
    tcpConnectedDisplay(tcpPort::SIG, false);
}

void MainWindow::slot_tcpConnected_msg()
{
    qDebug() << "slot_tcpConnected_msg";
    tcpConnectedDisplay(tcpPort::MSG, true);
    sendCommand("User\n\r");
}

void MainWindow::slot_tcpConnected_signal()
{
    qDebug() << "slot_tcpConnected_signal";
    tcpConnectedDisplay(tcpPort::SIG, true);
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
    tcpConnectedDisplay(tcpPort::SIG, false);
}

void MainWindow::tcpConnectedDisplay(tcpPort port, bool isConnected)
{
    qDebug()<<"tcpConnectedDisplay()";
    lbl_connection->hide();
    progbar_connecting_msg->hide();//hide it as common progbar_connecting
    lbl_port_msg->show();
    lbl_port_msg->raise();
    lbl_port_signal->show();
    lbl_port_signal->raise();

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
            act_record    ->setEnabled(false);
            stop();
        }
        lbl_port_msg->adjustSize();
        progbar_connecting_msg->hide();
        act_on_off->setEnabled(isConnected);
        act_playlist  ->setEnabled(isConnected);
        act_EegSessionSet       ->setEnabled(isConnected);

        if(d_tcpSocket_signal->state() == QAbstractSocket::ConnectingState){
            lbl_port_signal->setText(tr("Connection.."));
            lbl_port_signal->adjustSize();
            lbl_port_signal->setStyleSheet("color: black");
            progbar_connecting_signal->show();
            return;
        }
        break;

    case tcpPort::SIG:
        if(isConnected)
        {
            lbl_port_signal->setText(tr(((QString::number(d_port_signal) + connectedString).toStdString()).c_str()));
            lbl_port_signal->setStyleSheet("color: green");
            statusBar()->showMessage(tr(((QString::number(d_port_signal) + connectedString).toStdString()).c_str()), 2000);
        }
        else
        {
            lbl_port_signal->setStyleSheet("color: red");
            act_record    ->setEnabled(false);
        }

        lbl_port_signal->adjustSize();
        progbar_connecting_signal->hide();
        if(d_tcpSocket_msg->state() == QAbstractSocket::ConnectingState){
            lbl_port_msg->setText(tr("Connection.."));
            lbl_port_msg->adjustSize();
            lbl_port_msg->setStyleSheet("color: black");
            progbar_connecting_msg->resize(lbl_port_msg->size());
            progbar_connecting_msg->show();

            return;
        } /*else{
            act_start_stop->setEnabled(false);
        }*/
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
    QString addedText = msg;
    //EegSession eegSession;
    //std::string eegSession_str = radar::get_type_name(eegSession);
    //QString eegSessionName_str = QString::fromStdString(radar::get_type_name(d_lastEegSession));

    if(msg.startsWith(SESSION)){
        //addedText = msg;
        QString jsonMsg = msg;
        int ind_message_delimiter = jsonMsg.indexOf(message_delimiter);
        jsonMsg.remove(0, ind_message_delimiter+1);
        jsonMsg.remove(message_ending);
        qDebug() << jsonMsg;
        std::string msg_std_str = jsonMsg.toStdString();

        if(d_lastEegSession.from_json(msg_std_str)/*radar::from_json(msg_std_str, d_lastEegSession)*/)
        {
            displaySignalSettings();
            start();
        }
    }
//    if(msg.startsWith(textMessage_beginning)){
//        addedText = msg;
//    }
    if(msg.startsWith(textError_beginning)){
        addedText = QString("<span style=\" color:#ff0000;\">%1</span>").arg(addedText);
    }
    if(msg.startsWith(textWarning_beginning)){
        addedText = QString("<span style=\" color:#ff8c00;\">%1</span>").arg(addedText);
    }
//    if(msg.startsWith(d_cmdStrings_map.value(TurnOn){
//        act_game
//    }
    if(msg.startsWith(msgPlaylist_beginning)){
        qDebug() << addedText;
        QString fileName = chooseFromJsonArray(msg, "Playlist");
        if(!fileName.isEmpty())
            sendCommand(d_cmdStrings_map.value(Choose)+QString(message_delimiter)+fileName+message_ending);
    }
    if(msg.startsWith(d_cmdStrings_map.value(Record)))
    {
        if(msg.contains(RECORD_FINISHED))
        {
            QCommonStyle style;
            act_record->setIcon(QIcon(":/img/play.png"));
            act_record->setText(d_cmdStrings_map.value(Record));
            d_recordStarted = false;
        }
        if(msg.contains(ACCEPTED))
        {
            QCommonStyle style;
            act_record->setIcon(QIcon(":/img/stop.png"));
            act_record->setText(d_cmdStrings_map.value(Stop));
            d_recordStarted = true;
        }
    }
    if(msg.startsWith(d_cmdStrings_map.value(User)))
    {
        if(msg.contains(ACCEPTED))
        {
            QMessageBox::information(0, tr("User"), "Hi," + d_userName + "!");
        } else {
            d_userName = chooseFromJsonArray(msg, "Choose username");
            sendCommand(d_cmdStrings_map.value(User)+QString(message_delimiter)+d_userName+message_ending);
        }
    }
    if(msg.startsWith(d_cmdStrings_map.value(Game)))
    {
        if(d_gameState == GameState::Started)
        {
            if(msg.contains(ACCEPTED))
            {
                act_game->setDisabled(true);
            } else {
                QString chosenTrack = chooseFromJsonArray(msg, "Choose track");
                if(!chosenTrack.isEmpty())
                {
                    d_gameUserAnswer = d_cmdStrings_map.value(Game)+QString(message_delimiter)+chosenTrack+message_ending;
                    sendCommand(d_gameUserAnswer);
                    d_gameState = GameState::WaitingForGameAnswer;
                    //d_isWaitingForGameAnswer = true;
                }
            }
        }
        else{
            if(d_gameState == GameState::WaitingForGameAnswer)
            {
                if(d_gameUserAnswer == msg)
                {
                    QMessageBox::information(0, tr("Game result"), tr("RIGHT!"));
                } else {
                    QMessageBox::information(0, tr("Game result"), tr("WRONG!"));
                }
                d_gameState = GameState::Finished;
                act_game->setEnabled(true);
            }
        }
    }
    d_pte_NeuroslaveMsg->append(addedText);
}

QString MainWindow::chooseFromJsonArray(const QString & jsonArray_str, const QString &windowTitle)
{
    qDebug()<<"chooseFromJsonArray";
    QString jA_str = jsonArray_str;
    int ind_message_delimiter = jA_str.indexOf(message_delimiter);
    jA_str.remove(0, ind_message_delimiter+1);
    jA_str.remove(message_ending);
    qDebug() << jA_str;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(jA_str.toUtf8());
    if(jsonDoc.isNull())
        return "";
    if(jsonDoc.isArray())
    {
        QJsonArray jsonArray = jsonDoc.array();
        QVariantList list_qvariant = jsonArray.toVariantList();
        QStringList list_stringList;
        foreach(QVariant user, list_qvariant)
        {
            list_stringList << user.toString();
        }
        StringList_Dialog listDialog(list_stringList, windowTitle);
        if(listDialog.exec())
        {
            QString line = listDialog.chosenLine();
            qDebug() << "line" << line;
            return line;
            //sendCommand(d_cmdStrings_map.value(User)+QString(message_delimiter)+userName+message_ending);
        }
    }
    return "";
}

void MainWindow::slot_readTCP_signal()
{
    qDebug()<<"slot_readTCP_signal";
    if(!d_sessionStarted)
        return;
    qDebug() << "d_tcpSocket_signal->bytesAvailable()" << d_tcpSocket_signal->bytesAvailable();

    QDataStream in(d_tcpSocket_signal);

    int bytesAvailable = d_tcpSocket_signal->bytesAvailable();
    bool isLittleEndian = false;

    while(bytesAvailable > 0)
    {
        if(d_isPayloadReceived){
            EegSampleHeader header;
            //uint32_t header;
            in >> header.label;
            qDebug() << "header.Label"<< header.label;            
            bytesAvailable -= sizeof(header.label);

            if(header.label != NeuroslaveLabel)
            {
                if(header.label == qToBigEndian(NeuroslaveLabel))
                {
                    isLittleEndian = true;
                    header.label = qFromBigEndian(header.label);
                }
                else
                    return;
            }
            in >> header.n_channels;
            qDebug() << "header.n_channels"<< header.n_channels;
            bytesAvailable -= sizeof(header.n_channels);
            if(header.n_channels != d_lastEegSession.n_channels)
                return;
            in >> header.n_samples;
            qDebug() << "header.n_samples"<< header.n_samples;
            bytesAvailable -= sizeof(header.n_samples);
            if(bytesAvailable < header.n_channels * header.n_samples){
                qDebug() << "Only part of payload is availabale";
                return;
            }
            d_averageDecimation = header.n_samples;
            //d_displayDecimation = d_averageDecimation*DECIMATION;
            QVector<QVector<int32_t>> temp_points(header.n_samples); //collect d_averageDecimation points for every channel
            for(uint8_t j = 0; j < header.n_samples; j++)
            {
                temp_points[j].resize(header.n_channels);
                for(uint8_t i = 0; i < header.n_channels; i++)
                {
                    int32_t point;
                    in >> point;
                    if(isLittleEndian)
                        point = qFromBigEndian(point);
                    temp_points[j][i] = point;
                    qDebug() << "point"<< point;
                    bytesAvailable -= sizeof(point);
                }
            }
            
            for(uint8_t i = 0; i < header.n_channels; i++){
                double pointsSum = 0;
                //bool badSamle = false;
                uint pointCount = d_averageDecimation;
                //d_points[i].reserve(DECIMATION);
                //d_badSamples_xIndexes[i].reserve(d_averageDecimation);
                
                for(uint8_t j = 0; j < header.n_samples; j++) {
                    if(temp_points[j][i] == BADSAMLE)
                    {
                        pointCount--;
                        qDebug() << i << j << "BADSAMLE";
                        continue;
                    }
                    pointsSum += static_cast<double>(temp_points[j][i]);
                    qDebug() <<  i << j << "pointsSum" << pointsSum;
                }
                if(pointCount == 0)
                {
                    d_points[i].push_back(0);
                    d_badSamples_xIndexes[i].push_back(iXSignal*DECIMATION + d_points[i].size()-1);
                    qDebug() <<  i << "xIndex" << iXSignal*DECIMATION + d_points[i].size();
                            
                } else {
                    //d_badSamples[i].push_back(false);
                    d_points[i].push_back(pointsSum/pointCount);
                }              
                qDebug() <<  i << "d_points[i]" <<d_points[i].at(d_points[i].size()-1);
            }

            if(d_points.at(0).size() == static_cast<int>(DECIMATION)){
                QVector<double> x(DECIMATION);
                for(uint i = 0; i < DECIMATION; i++)
                {
                     x[i] = iXSignal*DECIMATION + i;

                }
                addData(x, d_points);
                //setBadSamples();
                d_points.clear();
                d_points.resize(d_lastEegSession.n_channels);
                d_badSamples_xIndexes.clear();
                d_badSamples_xIndexes.resize(d_lastEegSession.n_channels);

                for (uint i = 0; i < d_lastEegSession.n_channels; i++) {
                    d_points[i].reserve(DECIMATION);
                    d_badSamples_xIndexes[i].reserve(DECIMATION);
                }
                iXSignal++;
            }
        }

//        uint8_t state;
//        in >> state;
//        header.state = static_cast<NeuroslaveSampleState>(state);
//        switch (header.state) {
//        case NeuroslaveSampleState::GOOD:
//            //FILL ME
//            break;
//        case NeuroslaveSampleState::INDEX_ERROR:
//            d_pte_NeuroslaveMsg->append(QString("<span style=\" color:#ff0000;\">%1</span>").arg("NeuroslaveSampleState = INDEX_ERROR"));
//            break;
//        default:
//            break;
//        }
//        bytesAvailable -= sizeof(header.state);
//        if(bytesAvailable < header.payload_length)
//            return;
        //QVector<int32_t> points(header.payload_length);
//        for(uint8_t i = 0; i < header.payload_length; i++)
//        {
//            int32_t point;
//            in >> point;
//            d_points[i].push_back(static_cast<double>(point));
//            qDebug() << "point"<< point;
//            bytesAvailable -= sizeof(point);
//        }
//        if(d_points.at(0).size() == DISPLAY_DECIMATION){
//            QVector<double> x(DISPLAY_DECIMATION);
//            for(uint i = 0; i < DISPLAY_DECIMATION; i++)
//            {
//                x[i] = iXSignal*DISPLAY_DECIMATION + i;
//            }
//            addData(x, d_points);
//            d_points.clear();
//            d_points.resize(d_lastEegSession.n_channels);

//            for (uint i = 0; i < d_lastEegSession.n_channels; i++) {
//                d_points[i].reserve(DISPLAY_DECIMATION);
//            }
//            iXSignal++;
//        }
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
    d_errorGraphs.clear();
    d_errorGraphs.resize(d_lastEegSession.n_channels);

    d_points.clear();
    d_points.resize(d_lastEegSession.n_channels);
    d_badSamples_xIndexes.clear();
    d_badSamples_xIndexes.resize(d_lastEegSession.n_channels);

    for (uint i = 0; i < d_lastEegSession.n_channels; i++) {
        d_points[i].reserve(DECIMATION);
        d_badSamples_xIndexes[i].reserve(DECIMATION);
    }
    iXSignal = 0;
    //QList<QCPAxis*> allAxes;
    for(uint i = 0; i < d_lastEegSession.n_channels; i++)
    {
        QCPAxisRect* axisRect = new QCPAxisRect(d_signalPlot);
        d_signalPlot->plotLayout()->addElement(i,0,axisRect);

        //d_allAxes << axisRect->axes();
        d_xAxes << axisRect->axis( QCPAxis::atBottom);
        d_graph[i] = d_signalPlot->addGraph(axisRect->axis(QCPAxis::atBottom), axisRect->axis(QCPAxis::atLeft));
        if(d_graph[i] == nullptr)
            qDebug()<<"d_graph["<<i<<"] is nullptr";
        d_graph[i]->setPen(QPen(Qt::blue,2));

        d_errorGraphs[i] = d_signalPlot->addGraph(axisRect->axis(QCPAxis::atBottom), axisRect->axis(QCPAxis::atLeft));
        if(d_errorGraphs[i] == nullptr)
            qDebug()<<"d_errorGraphs["<<i<<"] is nullptr";
        d_errorGraphs[i]->setPen(QPen(Qt::red,2));
        d_errorGraphs[i]->setLineStyle(QCPGraph::lsNone);
        d_errorGraphs[i]->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 5));
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
//        if(d_badSamples[i].contains(true))
//        {
//            uint32_t badSamplesCount = d_badSamples[i].count(true);
//            int last_indBad = 0;
//            int cur_indBad = d_badSamples[i].indexOf(true);
//            if()
//            QVector
//        } else {
            d_graph[i]->addData(x,addedPoints.at(i));
            QVector<double> badSamples_yIndexes(d_badSamples_xIndexes[i].size(),0);
            d_errorGraphs[i]->addData(d_badSamples_xIndexes[i],badSamples_yIndexes);
            //d_graph[i]->rescaleAxes();
            qDebug()<<"addedPoints.at("<<i<<")" << addedPoints.at(i);
            //qDebug()<<"iXSignal" << iXSignal;
      //  }
    }
    //signal moving
    //setKeyRange


    if((iXSignal+1)*DECIMATION > signalSize){
//        for(uint i = 0; i < d_lastEegSession.n_channels; i++)
//        {

//            d_xAxes.at(i)->setRange((iXSignal+1)*DECIMATION - signalSize, (iXSignal+1)*DECIMATION);
//        }
        for (uint i = 0; i < d_lastEegSession.n_channels; i++) {
            QSharedPointer<QCPGraphDataContainer> graphData = d_graph.at(i)->data();
            graphData->removeBefore((iXSignal+1)*DECIMATION - signalSize);
            d_graph[i]->setData(graphData);
            //d_graph[i]->rescaleAxes();
            QSharedPointer<QCPGraphDataContainer> errorGraphData = d_errorGraphs.at(i)->data();
            errorGraphData->removeBefore((iXSignal+1)*DECIMATION - signalSize);
            d_errorGraphs[i]->setData(errorGraphData);
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

void MainWindow::setBadSamples()
{

}

void MainWindow::writeSettings()
{
    qDebug() << "writeSettings()";
    QSettings settings("Neuroslave", "EEG_GUI");
    settings.beginGroup("serverAddress");
    settings.setValue("Login", d_login);
    settings.setValue("Password", d_password);
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
    d_login = settings.value("serverAddress/Login","").toString();
    d_password = settings.value("serverAddress/Password","").toString();
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
