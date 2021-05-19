#ifndef DIALOGCONNECTIONSETTINGS_H
#define DIALOGCONNECTIONSETTINGS_H

#include <QDialog>
#include <QRegExpValidator>

namespace Ui {
class DialogConnectionSettings;
}

class DialogConnectionSettings : public QDialog
{
    Q_OBJECT
    QString d_serverIP;
    quint16 d_port_msg;
    quint16 d_port_sig;

public:
    explicit DialogConnectionSettings(QWidget *parent = 0);
    ~DialogConnectionSettings();
    QString serverIP();
    quint16 serverPort_msg();
    quint16 serverPort_signal();


    void setIp(QString);
    void setPort_msg(quint16);
    void setPort_signal(quint16);

private slots:
//    void slot_setIp();
//    void slot_setPort_msg();
//    void slot_setPort_sig();

private:
    Ui::DialogConnectionSettings *ui;

};

#endif // DIALOGCONNECTIONSETTINGS_H

