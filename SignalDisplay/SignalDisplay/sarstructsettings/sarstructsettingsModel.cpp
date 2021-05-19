#include "sarstructsettingsModel.h"

//SarStructSettingsModel::SarStructSettingsModel(const QString str_lastSarSession, QObject *parent)
//    : QAbstractItemModel(parent)/*, rootItem(nullptr)*/
//{
//    //initialize();
//    qDebug() << "SarStructSettingsModel::SarStructSettingsModel";
//    rootItem = new SarSessionItem({"", ""});
//    //QString str = "one one one \n two \t two two \n three \n four";
//    setUpModelData(str_lastSarSession, rootItem);
//    //addItem({tr("Парамет"), tr("Значени")}, rootItem);
//    //setData()
//}

//void SarStructSettingsModel::initialize()
//{
//    insertColumns(0,2);
//    //insertColumns()
//    //setHeaderData(0, Qt::Horizontal, tr("Параметр"));
//    //setHeaderData(1, Qt::Horizontal, tr("Значение"));
//}

//QVariant SarStructSettingsModel::headerData(int section, Qt::Orientation orientation, int role) const
//{
//    qDebug() << "SarStructSettingsModel::headerData";
//    if (orientation == Qt::Horizontal && role == Qt::DisplayRole){
//        if(section == static_cast<int>(Column::Parameter))
//            return tr("Параметр");
//        else if (section == static_cast<int>(Column::Value))
//            return tr("Значение");
//    }
//    return QVariant();
//}

////bool SarStructSettingsModel::setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role)
////{
////    if (value != headerData(section, orientation, role)) {
////        // FIXME: Implement me!
////        emit headerDataChanged(orientation, section, section);
////        return true;
////    }
////    return false; //headers can't be changed
////}

//QModelIndex SarStructSettingsModel::index(int row, int column, const QModelIndex &parent) const
//{
////    if (!rootItem || row < 0 || column < 0 || column >= ColumnCount || (parent.isValid() && parent.column() != 0))
////        return QModelIndex();
////    SarSessionItem *parentItem = itemForIndex(parent);
////    Q_ASSERT(parentItem);
////    if (SarSessionItem *item = parentItem->childAt(row))
////        return createIndex(row, column, item);
////    return QModelIndex();
//    qDebug() << "SarStructSettingsModel::index";
//    if (!hasIndex(row, column, parent))
//        return QModelIndex();
//    SarSessionItem *parentItem;
//    if(!parent.isValid())
//        parentItem = rootItem;
//    else
//        parentItem = static_cast<SarSessionItem*>(parent.internalPointer());

//    if (SarSessionItem *item = parentItem->childAt(row))
//        return createIndex(row, column, item);
//    return QModelIndex();
//}

//QModelIndex SarStructSettingsModel::parent(const QModelIndex &index) const
//{
//    qDebug() << "SarStructSettingsModel::parent";
////    if (!index.isValid())
////        return QModelIndex();
////    if (SarSessionItem *childItem = itemForIndex(index)) {
////        if (SarSessionItem* parentItem = childItem->parent()){
////            if (parentItem == rootItem)
////                return QModelIndex();
////            if (SarSessionItem* grandParentItem = parentItem->parent()){
////                int row = grandParentItem->rowOfChild(parentItem);
////                return createIndex(row, 0 , parentItem);
////            }
////        }
////    }
////    return QModelIndex();
//    if (!index.isValid())
//        return QModelIndex();
//    SarSessionItem *childItem = static_cast<SarSessionItem*>(index.internalPointer());
//    SarSessionItem *parentItem = childItem->parent();

//    if (parentItem == rootItem)
//        return QModelIndex();

//    return createIndex(parentItem->row(), 0 , parentItem);
//}

//int SarStructSettingsModel::rowCount(const QModelIndex &parent) const
//{
//    qDebug() << "SarStructSettingsModel::rowCount";
////    if (!parent.isValid() && parent.column() != 0)
////        return 0;
////    SarSessionItem *parentItem = itemForIndex(parent);
////    return parentItem ? parentItem->childCount() : 0;



//    SarSessionItem *parentItem;
//    if (parent.column() > 0)
//        return 0;
//    if (!parent.isValid())
//        parentItem = rootItem;
//    else
//        parentItem = static_cast<SarSessionItem*>(parent.internalPointer());

//    return parentItem->childCount();

////    SarSessionItem *parentItem = itemFromIndex(parent);
////    if(!parentItem)
////        return 0;
////    return parentItem->childCount();

//}

//int SarStructSettingsModel::columnCount(const QModelIndex &parent) const
//{
//    //return parent.isValid() && parent.column() != 0 ? 0 : ColumnCount;
//    qDebug() << "SarStructSettingsModel::columnCount";
//    if(parent.isValid())
//        return static_cast<SarSessionItem*>(parent.internalPointer())->columnCount();
//    return rootItem->columnCount();
//}

////bool SarStructSettingsModel::hasChildren(const QModelIndex &parent) const
////{
////    qDebug() << "SarStructSettingsModel::hasChildren";
////    // FIXME: Implement me!
////}

////bool SarStructSettingsModel::canFetchMore(const QModelIndex &parent) const
////{
////    qDebug() << "SarStructSettingsModel::canFetchMore";
////    // FIXME: Implement me!
////    return false;
////}

////void SarStructSettingsModel::fetchMore(const QModelIndex &parent)
////{
////    qDebug() << "SarStructSettingsModel::fetchMore";
////    // FIXME: Implement me!
////}

//QVariant SarStructSettingsModel::data(const QModelIndex &index, int role) const
//{
//    qDebug() << "SarStructSettingsModel::data";
//    if (!rootItem || !index.isValid() || index.column() < 0 || index.column() >=ColumnCount)
//        return QVariant();

//    if(SarSessionItem* item = itemForIndex(index)){
//        if (role == Qt::DisplayRole || role == Qt::EditRole){
//            switch (index.column()) {
//            case static_cast<int>(Column::Parameter):
//                return item->name();
//                break;
//            case static_cast<int>(Column::Value):
//               // return ;  // FIXME: Implement me!
//                break;
//            default: Q_ASSERT(false);
//            }
//        }
//        if (role == Qt::CheckStateRole && index.column() == static_cast<int>(Column::Parameter)){
//            return static_cast<int>(item->isChanged() ? Qt::Checked : Qt::Unchecked);
//        }
//        if (role == Qt::TextAlignmentRole){
//            if (index.column() == static_cast<int>(Column::Parameter)){
//                return static_cast<int>(Qt::AlignCenter|Qt::AlignLeft);
//            }
//            return static_cast<int>(Qt::AlignCenter|Qt::AlignRight);
//        }
//        //if (role == Qt::DecorationRole && index.column() == static_cast<int>(Column::Value)) // FIXME: Implement me!
//    }
//    return QVariant();




////    qDebug() << "SarStructSettingsModel::data";
////    if (!index.isValid())
////        return QVariant();

////    if(role != Qt::DisplayRole)
////        return QVariant();

////    SarSessionItem* item = static_cast<SarSessionItem*>(index.internalPointer());
////    return item->data(index.column());
//}

//bool SarStructSettingsModel::setData(const QModelIndex &index, const QVariant &value, int role)
//{
//    qDebug() << "SarStructSettingsModel::setData";
////    if (data(index, role) != value) {
////        // FIXME: Implement me!
////        emit dataChanged(index, index, QVector<int>() << role);
////        return true;
////    }
//    if (!index.isValid() || index.column() != static_cast<int>(Column::Parameter)){
//        return false;
//    }
//    if (SarSessionItem* item = itemForIndex(index)){
//        if (role == Qt::EditRole)
//            item->setName(value.toString());
//        else if (role == Qt::CheckStateRole)
//            item->setChanged(value.toBool());
//        else
//            return false;
//        emit dataChanged(index, index);
//        return true;
//    }
//    return false;
//}

//Qt::ItemFlags SarStructSettingsModel::flags(const QModelIndex &index) const
//{
//    qDebug() << "SarStructSettingsModel::flags";
//    Qt::ItemFlags theFlags = QAbstractItemModel::flags(index);
//    if (index.isValid()){
//        theFlags |= Qt::ItemIsSelectable|Qt::ItemIsEnabled;
//        if(index.column() == static_cast<int>(Column::Value))
//            theFlags |= Qt::ItemIsUserCheckable|Qt::ItemIsEditable/*|Qt::ItemIsDragEnabled|Qt::ItemIsDropEnabled*/;
//    }

//    return theFlags;
//}

//SarSessionItem *SarStructSettingsModel::itemForIndex(const QModelIndex& index) const
//{
//    qDebug() << "SarStructSettingsModel::itemForIndex";
//    if(index.isValid()){
//        if(SarSessionItem* item = static_cast<SarSessionItem*>(index.internalPointer()))
//            return item;
//    }
//    return rootItem;
//}

//SarSessionItem *SarStructSettingsModel::itemFromIndex(const QModelIndex& index) const
//{
//    qDebug() << "SarStructSettingsModel::itemFromIndex";
//    if(index.isValid()){
//        return static_cast<SarSessionItem*>(index.internalPointer());
//    }
//    else{
//        return rootItem;
//    }
//}


SarStructSettingsModel::SarStructSettingsModel(/*const QStringList &headers, */const QString &data, QObject *parent)
    : QAbstractItemModel(parent) {
    qDebug()<<"SarStructSettingsModel::SarStructSettingsModel";
 QVector <QVariant> rootData;
 rootData << "Parameter" << "Value"; //В модели будет 2 столбца
 rootItem = new SarSessionItem(rootData);
  //Создали корневой элемент
 setupModelData(data, rootItem);
  //Данные о строках модели разделены переводом строки
}

SarStructSettingsModel::~SarStructSettingsModel() { qDebug()<<"SarStructSettingsModel::~SarStructSettingsModel"; delete rootItem; }

int SarStructSettingsModel::columnCount(const QModelIndex &/*parent*/) const {
    qDebug()<<"SarStructSettingsModel::columnCount";
 return rootItem->columnCount();
}

QVariant SarStructSettingsModel::data (const QModelIndex &index, int role) const {
    qDebug()<<"SarStructSettingsModel::data";
 if (!index.isValid()) return QVariant();
 if (role == Qt::DisplayRole  || role == Qt::EditRole ) {
     SarSessionItem *item = getItem(index);
     return item->data(index.column());
 }
 else{
     if (role == Qt::BackgroundRole){
        if(d_changedInd.contains(index))
         return QColor(Qt::cyan);
     }
 }
 return QVariant();
}

SarSessionItem *SarStructSettingsModel::getItem(const QModelIndex &index) const {
    qDebug()<<"SarStructSettingsModel::getItem";
 if (index.isValid()) {
  SarSessionItem *item = static_cast<SarSessionItem*>(index.internalPointer());
  if (item) return item;
 }
 return rootItem;
}

Qt::ItemFlags SarStructSettingsModel::flags(const QModelIndex &index) const {
    qDebug()<<"SarStructSettingsModel::flags";
 if (!index.isValid()) return Qt::NoItemFlags;
 return Qt::ItemIsEditable | QAbstractItemModel::flags(index);
}

QVariant SarStructSettingsModel::headerData(int section, Qt::Orientation orientation, int role) const {
    qDebug()<<"SarStructSettingsModel::headerData";
 if (orientation == Qt::Horizontal && role == Qt::DisplayRole) return rootItem->data(section);
 return QVariant();
}

QModelIndex SarStructSettingsModel::index(int row, int column, const QModelIndex &parent) const {
    qDebug()<<"SarStructSettingsModel::index";
 if (parent.isValid() && parent.column() != 0) return QModelIndex();
 SarSessionItem *parentItem = getItem(parent);
 SarSessionItem *childItem = parentItem->child(row);
 if (childItem) return createIndex(row, column, childItem);
 else return QModelIndex();
}

QModelIndex SarStructSettingsModel::parent(const QModelIndex &index) const {
    qDebug()<<"SarStructSettingsModel::parent";
 if (!index.isValid()) return QModelIndex();
 SarSessionItem *childItem = getItem(index);
 SarSessionItem *parentItem = childItem->parentItem();
 if (parentItem == rootItem) return QModelIndex();
 return createIndex(parentItem->childNumber(), 0, parentItem);
}

int SarStructSettingsModel::rowCount(const QModelIndex &parent) const {
    qDebug()<<"SarStructSettingsModel::rowCount";
 SarSessionItem *parentItem = getItem(parent);
 return parentItem->childCount();
}

void SarStructSettingsModel::setupModelData(const QString &str_lastSarSession, SarSessionItem *parent) {
    qDebug() << "SarStructSettingsModel::setUpModelData_start";
    qDebug() << str_lastSarSession;

    int pos = 0;
    qDebug() << ".lastIndexOf(})" << str_lastSarSession.lastIndexOf("}");
    while (pos != str_lastSarSession.lastIndexOf("}")){
        qDebug() << "POS" << pos;
        pos = setupModelDataLevel(str_lastSarSession, pos, parent);
    }
}
//pos - current position in str
int SarStructSettingsModel::setupModelDataLevel(const QString &str, int pos, SarSessionItem *parent)
{
    qDebug() << "setupModelDataLevel" << pos;
    char message_delimiter = ':';
    int paramBeginInd = pos; //begining of parameter string
    int paramEndInd = paramBeginInd; //ending of parameter string


    int valBeginInd = paramBeginInd; //begining of value string
    int valEndInd = paramBeginInd; //ending of value string

    do { //splitter ","
        paramBeginInd = str.indexOf("\"", pos)+1; //parameter string is in ""
        paramEndInd = str.indexOf("\"", paramBeginInd);

        qDebug() <<  paramBeginInd << paramEndInd << str.mid(paramBeginInd, paramEndInd - paramBeginInd);
        valBeginInd = str.indexOf(message_delimiter, paramEndInd)+1; //value is after radar::message_delimiter
        qDebug() << str.mid(valBeginInd, 1);
        SarSessionItem* item = nullptr;
        pos = valBeginInd;
        if(str.mid(pos, 1) == "{"){ //if it is substruct        
            qDebug() << "SUBSTRUCT";
            QVector <QVariant> rowData({str.mid(paramBeginInd, paramEndInd-paramBeginInd)});
            item = new SarSessionItem(rowData, parent);
            parent->appendChild(item);            
            pos = setupModelDataLevel(str, pos, item);
        }
        else{
            valEndInd = str.indexOf(QRegExp("[,}]"), valBeginInd);

            QVector <QVariant> rowData({str.mid(paramBeginInd, paramEndInd-paramBeginInd), str.mid(valBeginInd, valEndInd-valBeginInd)});
            item = new SarSessionItem(rowData, parent);
            parent->appendChild(item);
            qDebug() << "3 " << str.mid(paramBeginInd, paramEndInd-paramBeginInd) << str.mid(valBeginInd, valEndInd-valBeginInd);
            pos = valEndInd;
            if(str.at(valEndInd) == "}"){
                qDebug() << "SUBSTRUCT_end";
                break;
            }
        }
    }while (pos != str.lastIndexOf("}"));
    qDebug() << "pos" << pos;
    return pos;
}

bool SarStructSettingsModel::setData(const QModelIndex &index, const QVariant &value, int role) {
    qDebug() << "SarStructSettingsModel::setData";
    if(index.column() == 1){
        qDebug() << value;

         if (role != Qt::EditRole) return false;
         SarSessionItem *item = getItem(index);
         bool result = item->setData(index.column(), value);

         if (result) {
            d_value_changed = value;
            d_changedInd << index;
            emit dataChanged(index, index);
         }
        // QFont font;
        // font.setBold(true);
        // setData(index,font,Qt::FontRole);
         return result;
    }
    return false;
}

bool SarStructSettingsModel::setHeaderData(int section, Qt::Orientation orientation,
                              const QVariant &value, int role) {
 if (role != Qt::EditRole || orientation != Qt::Horizontal) return false;
 bool result = rootItem->setData(section, value);
 if (result) {
  emit headerDataChanged(orientation, section, section);
 }
 return result;
}

bool SarStructSettingsModel::insertColumns(int position, int columns, const QModelIndex &parent) {
 bool success;
 beginInsertColumns(parent, position, position + columns - 1);
 success = rootItem->insertColumns(position, columns);
 endInsertColumns();
 return success;
}

bool SarStructSettingsModel::insertRows(int position, int rows, const QModelIndex &parent) {
 SarSessionItem *parentItem = getItem(parent);
 bool success;
 beginInsertRows(parent, position, position + rows - 1);
 success = parentItem->insertChildren(position, rows, rootItem->columnCount());
 endInsertRows();
 return success;
}

bool SarStructSettingsModel::removeColumns(int position, int columns, const QModelIndex &parent) {
 bool success;
 beginRemoveColumns(parent, position, position + columns - 1);
 success = rootItem->removeColumns(position, columns);
 endRemoveColumns();
 if (rootItem->columnCount() == 0) removeRows(0, rowCount());
 return success;
}

bool SarStructSettingsModel::removeRows(int position, int rows, const QModelIndex &parent) {
 SarSessionItem *parentItem = getItem(parent);
 bool success = true;
 beginRemoveRows(parent, position, position + rows - 1);
 success = parentItem->removeChildren(position, rows);
 endRemoveRows();
 return success;
}
