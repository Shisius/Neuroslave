#include <QtGui>
#include "sarstructsettingsdialog.h"



//sarStructSettingsDialog::sarStructSettingsDialog(const sar::SarSession &sarSession, QWidget *parent) : QDialog(parent), d_sarSession(sarSession)
//{
//    resize(500, 1000);
//    //********SarSessionModelAndView*************//
//    createSarSessionModelAndView(sarSession);
//    d_pb_sendSettings = new QPushButton(tr("Отправить"));
//    connect(d_pb_sendSettings, SIGNAL(clicked()),SLOT(accept()));
//    d_pb_cancel = new QPushButton(tr("Отмена"));
//    connect(d_pb_cancel, SIGNAL(clicked()),SLOT(close()));
//    buttonLayout = new QHBoxLayout;
//    buttonLayout->addStretch();
//    buttonLayout->addWidget(d_pb_sendSettings);
//    buttonLayout->addWidget(d_pb_cancel);



//    mainLayout = new QVBoxLayout;
////createTreeWidget(sarSession);


//    setWindowTitle(tr("Параметры SarSession"));
//    //mainLayout->addWidget(treeWidget);

//    mainLayout->addWidget(d_sarSessionSettings_view);
//    mainLayout->addLayout(buttonLayout);
//    setLayout(mainLayout);
//    qDebug() << connect(d_sarSessionSettings_model, SIGNAL(dataChanged(const QModelIndex &, const QModelIndex &)), SLOT(slot_dataChanged(const QModelIndex &)));
//}

sarStructSettingsDialog::sarStructSettingsDialog(const QString& str_sarStruct, const QString& windowTitle, QWidget *parent) : QDialog(parent), str_sarStruct(str_sarStruct)
{
    resize(500, 500);
    //********SarStructModelAndView*************//
    createSarStructModelAndView();
    d_pb_sendSettings = new QPushButton(tr("Send"));
    connect(d_pb_sendSettings, SIGNAL(clicked()),SLOT(accept()));
    d_pb_cancel = new QPushButton(tr("Cancel"));
    connect(d_pb_cancel, SIGNAL(clicked()),SLOT(close()));
    buttonLayout = new QHBoxLayout;
    buttonLayout->addStretch();
    buttonLayout->addWidget(d_pb_sendSettings);
    buttonLayout->addWidget(d_pb_cancel);

    mainLayout = new QVBoxLayout;

    setWindowTitle(windowTitle);


    mainLayout->addWidget(d_sarStructSettings_view);
    mainLayout->addLayout(buttonLayout);
    setLayout(mainLayout);
    qDebug() << connect(d_sarStructSettings_model, SIGNAL(dataChanged(const QModelIndex &, const QModelIndex &)), SLOT(slot_dataChanged(const QModelIndex &)));
}

void sarStructSettingsDialog::slot_dataChanged(const QModelIndex &index)
{
    qDebug() << "slot_dataChanged" << index;
    char message_delimiter = ':';
    qDebug() << str_sarStruct;
    QModelIndex ind = index.sibling(index.row(),0);
    //qDebug() << ind;
    QVariant field = d_sarStructSettings_model->data(ind, Qt::DisplayRole);
    int indChangedData = str_sarStruct.indexOf(field.toString());
    int ival_beg = str_sarStruct.indexOf(message_delimiter, indChangedData)+1;
    int ival_end = str_sarStruct.indexOf(QRegExp("[,}]"), ival_beg);
    //qDebug() << ival_beg;
    //qDebug() << ival_end;
    QString newValue = d_sarStructSettings_model->value_changed().toString();
    //qDebug() << newValue;
    str_sarStruct.replace(ival_beg, ival_end-ival_beg, newValue);
    qDebug() << str_sarStruct;

}

QString sarStructSettingsDialog::changedSarStructString()
{
    return str_sarStruct;
}

//********SarStructModelAndView*************//
void sarStructSettingsDialog::createSarStructModelAndView()
{
    qDebug() << "sarStructSettingsDialog::createSarStructModelAndView";

     d_sarStructSettings_model  = new SarStructSettingsModel(/*headers, *//*file.readAll()*/str_sarStruct);
     //file.close();
     d_sarStructSettings_view = new QTreeView;
     d_sarStructSettings_view->setModel(d_sarStructSettings_model);
     for (int column = 0; column < d_sarStructSettings_model->columnCount(); ++column) d_sarStructSettings_view->resizeColumnToContents(column);
    d_sarStructSettings_view->expandAll();
    d_sarStructSettings_view->setAllColumnsShowFocus(true);
    d_sarStructSettings_view->setTabKeyNavigation(true);
    d_sarStructSettings_view->setEditTriggers(QAbstractItemView::AllEditTriggers);
    //qDebug() << connect(d_sarSessionSettings_view, SIGNAL(activated(const QModelIndex &)), SLOT(slot_entered(const QModelIndex &)));
    qDebug() << "createSarStructModelAndView_end";
}

void sarStructSettingsDialog::slot_entered(const QModelIndex &index)
{
    qDebug() << "slot_entered";
    QModelIndex ind = index.sibling(index.row(),1);
//    d_sarSessionSettings_view->edit(ind);
    d_sarStructSettings_view->setCurrentIndex(ind);
}

void sarStructSettingsDialog::setCurrentIndex(const QModelIndex &index)
{
    if(index.isValid()){
        d_sarStructSettings_view->scrollTo(index);
        d_sarStructSettings_view->setCurrentIndex(index);
    }
}

