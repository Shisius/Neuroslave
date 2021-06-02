#ifndef STRINGLIST_DIALOG_H
#define STRINGLIST_DIALOG_H

#include <QDialog>
#include <QtWidgets>

class StringList_Dialog : public QDialog
{
    Q_OBJECT
private:
    QStringListModel d_model;
    QListView* d_pListView;
    QPushButton* d_ppb_choose;
    QPushButton* d_ppb_cancel;


public:
    StringList_Dialog(const QStringList &, const QString &, QWidget *parent = 0);
    QString chosenLine();
};

#endif // STRINGLIST_DIALOG_H
