#include "sarsessionitem.h"

////SarSessionItem::SarSessionItem(const QString &name, bool changed, SarSessionItem* parent)
////    :d_name(name),d_changed(changed),d_parent(parent)
////{
////    if(d_parent)
////        d_parent->addChild(this);
////}

//SarSessionItem::SarSessionItem(const QVector<QVariant> &data, SarSessionItem* parent)
//    :d_itemData(data), d_parent(parent)
//{

//}

////SarSessionItem::SarSessionItem(SarSessionItem* parent)
////    : d_parent(parent)
////{
////    qDebug() << "SarSessionItem::SarSessionItem";
////}


SarSessionItem::SarSessionItem (const QVector<QVariant> &data, SarSessionItem *parent) {
 //Конструктору узла нужно передать данные и ссылку на родителя
 m_parentItem = parent;
 m_itemData = data;
}

SarSessionItem::~SarSessionItem() { qDeleteAll(m_childItems); }

/*
 Методы класса служат, по сути, интерфейсом к
 соответствующим методам стандартного класса QVector:
*/

void SarSessionItem::appendChild(SarSessionItem *item) {
 m_childItems.append(item);
 //Добавить узел в список потомков
}

SarSessionItem *SarSessionItem::child (int row) {
 return m_childItems.value(row);
 //По номеру строки выбрать нужного потомка из списка
}

int SarSessionItem::childCount() const {
 return m_childItems.count();
 //Количество потомков узла = длине списка потомков
}

int SarSessionItem::columnCount() const {
 return m_itemData.count();
 //Количество столбцов в узле = длине списка данных узла
}

QVariant SarSessionItem::data (int column) const {
 return m_itemData.value(column);
 //Взять данные из нужного столбца
}

SarSessionItem *SarSessionItem::parentItem() {
 return m_parentItem; //Вернуть ссылку на родителя
}

int SarSessionItem::childNumber() const {
 //Если есть родитель - найти свой номер в списке его потомков
 if (m_parentItem) return m_parentItem->m_childItems.indexOf(const_cast<SarSessionItem*>(this));
 return 0; //Иначе вернуть 0
}

/*
 Следующие 4 метода просто управляют контейнерами класса m_childItems и m_itemData,
 предназначенными для хранения данных
*/

bool SarSessionItem::insertChildren(int position, int count, int columns) {
 if (position < 0 || position > m_childItems.size()) return false;
 for (int row = 0; row < count; ++row) {
  QVector<QVariant> data(columns);
  SarSessionItem *item = new SarSessionItem(data, this);
  m_childItems.insert(position, item);
 }
 return true;
}

bool SarSessionItem::insertColumns(int position, int columns) {
 if (position < 0 || position > m_itemData.size()) return false;
 for (int column = 0; column < columns; ++column) m_itemData.insert(position, QVariant());
 foreach (SarSessionItem *child, m_childItems) child->insertColumns(position, columns);
 return true;
}

bool SarSessionItem::removeChildren(int position, int count) {
 if (position < 0 || position + count > m_childItems.size()) return false;
 for (int row = 0; row < count; ++row) delete m_childItems.takeAt(position);
 return true;
}

bool SarSessionItem::removeColumns(int position, int columns) {
 if (position < 0 || position + columns > m_itemData.size()) return false;
 for (int column = 0; column < columns; ++column) m_itemData.removeAt(position);
 foreach (SarSessionItem *child, m_childItems) child->removeColumns(position, columns);
 return true;
}

//А этот метод ставит значение value в столбец column элемента:
bool SarSessionItem::setData(int column, const QVariant &value) {
 if (column < 0 || column >= m_itemData.size()) return false;
 if (m_itemData[column] == value) return false;
 m_itemData[column] = value;
 return true;
}
