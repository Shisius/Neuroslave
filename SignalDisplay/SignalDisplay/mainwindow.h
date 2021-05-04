#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpSocket>
#include "qcustomplot.h"
#include "dialogconnectionsettings.h"

using signalSample_t = int32_t;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
private slots:
    void slot_connection();
    void slot_readTCP();
    void slot_catchErrorTCP(QAbstractSocket::SocketError);
    void slot_tcpConnected();
    void slot_tcpDisconnected();

private:
    QWidget *d_centralWidget;
    QLabel* lbl_connection = nullptr;
    QCustomPlot *d_signalPlot;
    QMenu* menu_settings;
    QAction* act_connection;

    DialogConnectionSettings *dialog_connectionSettings = nullptr;
    QString d_serverIP;
    quint16 d_port;
    QTcpSocket *d_tcpSocket = nullptr;
    QProgressBar *progbar_connecting = nullptr;

    double iXSignal = 0; //position on X axis for adding signal


    void createActions();
    void createMenus();

    void initConnection();

    void tcpConnect();
    void closeConnection();


    void displaySignalSettings();
    void addSignalToPlot(const std::vector<signalSample_t> &);

    void writeSettings();
    void readSettings();
protected:
    void closeEvent(QCloseEvent *event) override;
};
#endif // MAINWINDOW_H
