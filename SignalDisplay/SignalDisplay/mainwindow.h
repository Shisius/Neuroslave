#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpSocket>
#include "qcustomplot.h"

#include "neuroslave_struct.h"
#include "neuroslave_msg.h"
#include "dialogconnectionsettings.h"
//#include "struct_tools.h"
#include "sarstructsettings/sarstructsettingsdialog.h"

using signalSample_t = int32_t;

const QString message_ending = "\n\r";
const char message_delimiter = ':';
const QString textMessage_beginning = "Message";
const QString textError_beginning = "Error";
const QString textWarning_beginning = "Warning";
const QString msgPlaylist_beginning = "Playlist";
const QString RECORD_FINISHED = "Finished";
const QString ACCEPTED = "Accepted";
const QString SESSION = "Session";
const uint16_t NeuroslaveLabel = 0xACDC;
const uint signalSize = 1000;
const int32_t BADSAMLE = 0x7BADBAD7;
const uint DECIMATION = 100; //update graphs after receiving every DISPLAY_DECIMATION samples
//const uint AVERAGE_DECIMATION = 10; //display one sample counted as average of AVERAGE_DECIMATION samples

class MainWindow : public QMainWindow
{
    Q_OBJECT
//    struct EegSession {
//        std::string tag; // Session name
//        unsigned int sample_rate; // Sample rate in Hz
//        unsigned int n_channels; // Number of data channels. This value should be equal to number of plotted curves.
//        unsigned int n_samples_per_pack; // Number of samples in every binary pack
//        unsigned int gain; // Gain value for amplifier
//        unsigned int tcp_decimation; // Data decimation for tcp binary port
//        XTOSTRUCT(O(tag, sample_rate, n_channels, n_samples_per_pack, gain, tcp_decimation))
//    };
    
      

//    struct GameSettings {
//        std::string subfolder; // Subfolder in playlist folder
//        float duration; // in seconds
//        float volume; // from 0 to 100%
//        uint8_t complexity; // from 2 to 6. Number of music files for choose
//        XTOSTRUCT(O(subfolder, duration, volume, complexity))
//    };

    
//    enum class NeuroslaveSampleState : uint8_t {
//        GOOD = 0,
//        INDEX_ERROR = 1
//    };

//    struct Header{
//        uint16_t Label;
//        uint8_t payload_length;
//        NeuroslaveSampleState state;
//    };
    struct EegSampleHeader {
        unsigned short label;
        unsigned char n_channels;
        unsigned char n_samples;
    };


//    struct BinaryData {
//        struct Header header;
//        int32_t Payload
//    };


public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
private slots:
    void slot_connection();
    void slot_readTCP_msg();
    void slot_readTCP_signal();
    void slot_catchErrorTCP_msg(QAbstractSocket::SocketError);
    void slot_tcpConnected_msg();
    void slot_tcpDisconnected_msg();
    void slot_catchErrorTCP_signal(QAbstractSocket::SocketError);
    void slot_tcpConnected_signal();
    void slot_tcpDisconnected_signal();
    void slot_clearRadarMsgWindow();
    void slot_start();    
    void slot_EegSessionSet();
    void slot_chooseMusic();
    void slot_record();
    void slot_game();
    void slot_gameSet();

private:
    QWidget *d_centralWidget;
    QVBoxLayout *d_centralLayout;

    QMenu* menu_settings;    
    QAction* act_connection;

    //Tool bar:
    QAction* act_on_off;
    QAction* act_playlist;
    QAction* act_EegSessionSet;
    QAction* act_record;
    QAction* act_game;
    QAction* act_gameSet;

    //Bottom dock:
    QToolBar* d_ptb_NeuroslaveMsg;
    QLabel* d_plbl_NeuroslaveMsg;
    QDockWidget* d_pdock_NeuroslaveMsg;
    QTextEdit* d_pte_NeuroslaveMsg;

    //Connection
    QLabel* lbl_connection = nullptr;
    DialogConnectionSettings *dialog_connectionSettings = nullptr;
    QString d_serverIP;
    quint16 d_port_msg;
    quint16 d_port_signal;
    QString d_login;
    QString d_password;
    QTcpSocket *d_tcpSocket_msg = nullptr;
    QTcpSocket *d_tcpSocket_signal = nullptr;
    QProgressBar *progbar_connecting_msg = nullptr;
    QProgressBar *progbar_connecting_signal = nullptr;

    QLabel* lbl_port_msg = nullptr;
    QLabel* lbl_port_signal = nullptr;

    enum class tcpPort
    {
        MSG,
        SIG
    };

    //Graphs
    QCustomPlot *d_signalPlot;
    QList<QCPAxis*> d_xAxes;
    double iXSignal = 0; //position on X axis for adding signal
    QVector<QCPGraph*> d_graph;
    QVector<QCPGraph*> d_errorGraphs;
    QVector<QVector<double>> d_points;
    //QVector<QVector<bool>> d_badSamples;
    QVector<QVector<double>> d_badSamples_xIndexes;
    QString d_textMessage;

    NeuroslaveSession d_lastEegSession;

    bool d_sessionStarted = false;
    bool d_recordStarted = false;

    bool d_isPayloadReceived = true;
    int32_t d_PayloadReceived_count = 0;
    uint d_displayDecimation;
    uint d_averageDecimation;

    enum Cmd
    {
        TurnOn,
        Set,
        TurnOff,
        Choose,
        Record,
        Stop,
        User,
        Game
    };
    QMap<Cmd, QString> d_cmdStrings_map = {{TurnOn, "TurnOn"},
                                           {Set, "Set"},
                                           {TurnOff, "TurnOff"},
                                           {Choose, "Choose"},
                                           {Record, "Record"},
                                           {Stop, "Stop"},
                                           {User, "User"},
                                           {Game, "Game"}};
    bool d_isWaitingForGameAnswer = false;
    QString d_gameUserAnswer;
    QString d_userName;

    enum class GameState
    {
        Started,
        WaitingForGameAnswer,
        Finished
    } d_gameState;

    void createActions();
    void createMenus();
    void createDocks();
    QToolBar* createToolBar();

    void initConnection();
    //void initSignalReceiving();

    void tcpConnect();
    void closeConnections();
    void closeConnection_msg();
    void closeConnection_signal();
    void tcpConnectedDisplay(tcpPort port, bool isConnected);

    void displaySignalSettings();
    void addData(const QVector<double> &x, const QVector<QVector<double>> &addedPoints);
    void setBadSamples();
    void processMessage(const QString &msg);
    void sendCommand(const QString & cmd);
    void stop();
    void start();
    QString chooseFromJsonArray(const QString & jsonArray_str, const QString & windowTitle);

    void writeSettings();
    void readSettings();
protected:
    void closeEvent(QCloseEvent *event) override;
};
#endif // MAINWINDOW_H
