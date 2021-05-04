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
    quint16 d_port;

public:
    explicit DialogConnectionSettings(QWidget *parent = 0);
    ~DialogConnectionSettings();
    QString serverIP();
    quint16 serverPort();


    void setIp(QString);
    void setPort(quint16);

private slots:
    void slot_setIp();
    void slot_setPort();

private:
    Ui::DialogConnectionSettings *ui;

};

#endif // DIALOGCONNECTIONSETTINGS_H

