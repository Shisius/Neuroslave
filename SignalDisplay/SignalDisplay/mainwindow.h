#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpSocket>
#include "qcustomplot.h"
#include "dialogconnectionsettings.h"
#include "struct_tools.h"
#include "sarstructsettings/sarstructsettingsdialog.h"

using signalSample_t = int32_t;

const QString message_ending = "\n\r";
const char message_delimiter = ':';
const QString textMessage_beginning = "Message";
const QString textError_beginning = "Error";
const QString textWarning_beginning = "Warning";
const QString msgPlaylist_beginning = "Playlist";
const uint16_t NeuroslaveLabel = 0xACDC;
const uint signalSize = 1000;
const uint DECIMATION_KOEFF = 100;

class MainWindow : public QMainWindow
{
    Q_OBJECT
    struct EegSession {
        std::string tag; // Session name
        float sample_rate; // Sample rate in Hz
        unsigned int n_channels; // Number of data channels. This value should be equal to number of plotted curves.
        XTOSTRUCT(O(tag, sample_rate, n_channels))
    };

    struct Header{
        uint16_t Label;
        uint8_t payload_length;
        uint8_t reserved;
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
    void slot_set();
    void slot_chooseMusic();

private:
    QWidget *d_centralWidget;
    QVBoxLayout *layout;
    QLabel* lbl_connection = nullptr;
    QCustomPlot *d_signalPlot;
    QList<QCPAxis*> d_allAxes;
    QMenu* menu_settings;
    QAction* act_connection;

    QAction* act_start_stop;

    //Bottom dock:
    QToolBar* d_ptb_NeuroslaveMsg;
    QLabel* d_plbl_NeuroslaveMsg;
    QDockWidget* d_pdock_NeuroslaveMsg;
    QTextEdit* d_pte_NeuroslaveMsg;

    DialogConnectionSettings *dialog_connectionSettings = nullptr;
    QString d_serverIP;
    quint16 d_port_msg;
    quint16 d_port_signal;
    QTcpSocket *d_tcpSocket_msg = nullptr;
    QTcpSocket *d_tcpSocket_signal = nullptr;
    QProgressBar *progbar_connecting_msg = nullptr;
    QProgressBar *progbar_connecting_signal = nullptr;

    QLabel* lbl_port_msg = nullptr;
    QLabel* lbl_port_signal = nullptr;

    enum class tcpPort
    {
        MSG,
        SIGNAL
    };

    double iXSignal = 0; //position on X axis for adding signal

    QString d_textMessage;

    EegSession d_lastEegSession;

    QVector<QCPGraph*> d_graph;
    QVector<QVector<double>> d_points;

    bool d_sessionStarted = false;

    void createActions();
    void createMenus();
    void createDocks();
    QToolBar* createToolBar();

    void initConnection();

    void tcpConnect();
    void closeConnections();
    void closeConnection_msg();
    void closeConnection_signal();
    void tcpConnectedDisplay(tcpPort port, bool isConnected);

    void displaySignalSettings();
    void addData(const QVector<double> &x, const QVector<QVector<double>> &addedPoints);
    void processMessage(const QString &msg);
    void sendCommand(const QString & cmd);
    void stop();
    void start();

    void writeSettings();
    void readSettings();
protected:
    void closeEvent(QCloseEvent *event) override;
};
#endif // MAINWINDOW_H
