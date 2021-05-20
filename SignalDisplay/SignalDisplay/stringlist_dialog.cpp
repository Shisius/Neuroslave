#include "stringlist_dialog.h"

StringList_Dialog::StringList_Dialog(const QStringList & strList, QWidget *parent)

{
    d_model.setStringList(strList);
    d_ppb_choose     = new QPushButton("&Choose");
    d_ppb_cancel = new QPushButton("&Cancel");
    QHBoxLayout* buttonLayout = new QHBoxLayout;
    buttonLayout->addStretch();
    buttonLayout->addWidget(d_ppb_choose);
    buttonLayout->addWidget(d_ppb_cancel);

    QVBoxLayout* mainLayout = new QVBoxLayout;

    setWindowTitle(tr("Eegsession parameters"));

    d_pListView = new QListView;
    d_pListView->setModel(&d_model);
    d_pListView->setSelectionMode(QAbstractItemView::SingleSelection);
    mainLayout->addWidget(d_pListView);
    mainLayout->addLayout(buttonLayout);
    setLayout(mainLayout);
    connect(d_ppb_choose,    SIGNAL(clicked()), SLOT(accept()));
    connect(d_ppb_cancel,SIGNAL(clicked()), SLOT(reject()));

}

QString StringList_Dialog::fileName()
{
    return qvariant_cast<QString>(d_pListView->currentIndex().data());
}
