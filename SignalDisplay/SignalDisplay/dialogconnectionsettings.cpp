#include "dialogconnectionsettings.h"
#include "ui_dialogconnectionsettings.h"
#include <QDebug>

DialogConnectionSettings::DialogConnectionSettings(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogConnectionSettings)
{
    ui->setupUi(this);
    QRegExp re_ip("[0-9\b]{1,3}\\.[0-9\b]{1,3}\\.[0-9\b]{1,3}\\.[0-9\b]{1,3}");
    ui->le_ip->setValidator(new QRegExpValidator(re_ip, this));
    //connect(ui->le_ip, SIGNAL(textChanged(QString)), SLOT(slot_setIp()));
    QRegExp re_port("[0-9]{1,5}");
    ui->le_port_msg->setValidator(new QRegExpValidator(re_port, this));
    ui->le_port_sig->setValidator(new QRegExpValidator(re_port, this));
    //connect(ui->le_port, SIGNAL(textChanged(QString)), SLOT(slot_setPort_msg()));
    //ui->le_username->setS
}

DialogConnectionSettings::~DialogConnectionSettings()
{
    delete ui;
}

//void DialogConnectionSettings::slot_setIp()
//{
//    //qDebug() << "slot_setIp"<< d_serverIP;
//    d_serverIP = ui->le_ip->text().toLatin1().data();

//    //d_serverIP = ui->le_ip->text().data();
//}

//void DialogConnectionSettings::slot_setPort()
//{
//    d_port = ui->le_port->text().toUShort();
//}

void DialogConnectionSettings::setIp(const QString &ip)
{
    //QString ip_str = QString(ip);
    //qDebug() << 2<< ip_str;
    ui->le_ip->setText(ip);
}

void DialogConnectionSettings::setPort_msg(quint16 port)
{
    QString port_str;
    ui->le_port_msg->setText(port_str.setNum(port));
}

void DialogConnectionSettings::setPort_signal(quint16 port)
{
    QString port_str;
    ui->le_port_sig->setText(port_str.setNum(port));
}

void DialogConnectionSettings::setLogin(const QString &login)
{
    ui->le_login->setText(login);
}

void DialogConnectionSettings::setPassword(const QString &password)
{
    ui->le_password->setText(password);
}

QString DialogConnectionSettings::serverIP()
{
    qDebug() << "serverIP"<< ui->le_ip->text().toLatin1().data();
    return ui->le_ip->text().toLatin1().data();
}

quint16 DialogConnectionSettings::serverPort_msg()
{
    qDebug() << "port"<< ui->le_port_msg->text().toUShort();
    return ui->le_port_msg->text().toUShort();
}


quint16 DialogConnectionSettings::serverPort_signal()
{
    qDebug() << "port"<< ui->le_port_sig->text().toUShort();
    return ui->le_port_sig->text().toUShort();
}

QString DialogConnectionSettings::login()
{
    qDebug() << "login"<< ui->le_login->text().toLatin1().data();
    return ui->le_login->text().toLatin1().data();
}

QString DialogConnectionSettings::password()
{
    qDebug() << "password"<< ui->le_password->text().toLatin1().data();
    return ui->le_password->text().toLatin1().data();
}
