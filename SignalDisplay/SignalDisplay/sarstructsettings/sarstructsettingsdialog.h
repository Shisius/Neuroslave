#ifndef SARSTRUCTSETTINGSDIALOG_H
#define SARSTRUCTSETTINGSDIALOG_H

#include <QDialog>

#include "sarstructsettings/sarstructsettingsModel.h"
class QTreeView;
class SarStructSettingsModel;
class QPushButton;
class QHBoxLayout;
class QVBoxLayout;
class QTreeWidget;
class QSettings;
class QTreeWidgetItem;

class sarStructSettingsDialog : public QDialog
{
    Q_OBJECT
public:    
    sarStructSettingsDialog(const QString&, QWidget *parent = 0);
    QString changedSarStructString();
private slots:
    void slot_dataChanged(const QModelIndex &index);
    void slot_entered(const QModelIndex &);
private:    
    QPushButton *d_pb_sendSettings;
    QPushButton *d_pb_cancel;
    QHBoxLayout *buttonLayout;
    QVBoxLayout *mainLayout;

    QString str_sarStruct;

    //********SarStructModelAndView*************//
    QTreeView *d_sarStructSettings_view;
    SarStructSettingsModel *d_sarStructSettings_model = nullptr;
    void createSarStructModelAndView();
    void setCurrentIndex(const QModelIndex &index);
    //void readSettings();
};

#endif // SARSTRUCTSETTINGSDIALOG_H
