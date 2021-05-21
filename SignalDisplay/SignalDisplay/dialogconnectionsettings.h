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
    QString d_username;
    QString d_password;

public:
    explicit DialogConnectionSettings(QWidget *parent = 0);
    ~DialogConnectionSettings();
    QString serverIP();
    quint16 serverPort_msg();
    quint16 serverPort_signal();
    QString username();
    QString password();


    void setIp(const QString &);
    void setPort_msg(quint16);
    void setPort_signal(quint16);
    void setUsername(const QString &);
    void setPassword(const QString &);

private slots:
//    void slot_setIp();
//    void slot_setPort_msg();
//    void slot_setPort_sig();

private:
    Ui::DialogConnectionSettings *ui;

};

#endif // DIALOGCONNECTIONSETTINGS_H

