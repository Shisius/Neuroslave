#ifndef SARSESSIONITEM_H
#define SARSESSIONITEM_H

#include <QtWidgets>

//class SarSessionItem
//{
//private:
//    QString d_name;
//    bool d_changed;

//    QVector<SarSessionItem*> d_children;
//    QVector<QVariant> d_itemData;
//    SarSessionItem* d_parent;
//public:
//    //explicit SarSessionItem(const QString &name=QString(), bool changed = false, SarSessionItem* parent = 0);
//    SarSessionItem(const QVector<QVariant> &data, SarSessionItem* parent = 0);
//    //SarSessionItem(SarSessionItem* parent = 0);
//    ~SarSessionItem() {qDeleteAll(d_children);}
//    QString name() const {return d_name;}
//    void setName(const QString &name) {d_name = name;}
//    bool isChanged() const {return d_changed;}
//    void setChanged(bool changed) {d_changed = changed;}
//    SarSessionItem* parent() const {return d_parent;}
//    SarSessionItem* childAt(int row) const {return d_children.value(row);}
//    int rowOfChild(SarSessionItem* child) const {return d_children.indexOf(child);}

//    int childCount() const {return d_children.count();}
//    bool hasChildren() const {return !d_children.isEmpty();}
//    QVector<SarSessionItem*> children()const {return d_children;}
//    void insertChild(int row, SarSessionItem* item)
//        {item->d_parent = this; d_children.insert(row, item);}
//    void addChild(SarSessionItem *item)
//        {qDebug() << "addChild";/*item->d_parent = this; d_children << item;*/ d_children.append(item); }

//    int row() const {if(d_parent) return d_parent->d_children.indexOf(const_cast<SarSessionItem*>(this)); return 0;}
//    int columnCount() const {return d_itemData.count();}
//    QVariant data(int column) const {if(column < 0 || column >= d_itemData.size()) return QVariant(); return d_itemData.at(column);}
//};


class SarSessionItem {
public:
 explicit SarSessionItem (const QVector<QVariant> &data,
 SarSessionItem *parentItem = 0); //Конструктор узла дерева
 ~SarSessionItem();               //...и деструктор

 void appendChild(SarSessionItem *child); //Добавить узел-потомок
 SarSessionItem *child(int row); //Вернуть дочерний элемент
 int childCount() const; //Количество дочерних элементов
 int columnCount() const; //Вернуть количество столбцов элемента
 QVariant data(int column) const; //Вернуть данные указанного столбца
 int childNumber() const; //Вернуть номер строки элемента

 SarSessionItem *parentItem(); //Вернуть родительский элемент
 bool insertChildren(int position, int count, int columns); //Вставить потомков (строки)
 bool insertColumns(int position, int columns); //Вставить столбцы
 bool removeChildren(int position, int count);  //Удалить потомков
 bool removeColumns(int position, int columns); //Удалить столбцы
 bool setData(int column, const QVariant &value); //Установить данные
private: //Внутреннее представление данных:
 QList <SarSessionItem*> m_childItems; //Список дочерних элементов
 QVector <QVariant> m_itemData; //Список данных текущего узла
 SarSessionItem *m_parentItem; //Ссылка на родительский узел
};

#endif // SARSESSIONITEM_H
