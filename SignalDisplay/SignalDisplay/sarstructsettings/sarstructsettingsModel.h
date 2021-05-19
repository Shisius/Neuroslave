#ifndef SARSTRUCTSETTINGSMODEL_H
#define SARSTRUCTSETTINGSMODEL_H

#include <QAbstractItemModel>
#include "sarstructsettings/sarsessionitem.h"
//#include "sarstructsettings/sarstructparamdelegate.h"

enum class Column{Parameter, Value};
const int ColumnCount = 2;

//class SarStructSettingsModel : public QAbstractItemModel
//{
//    Q_OBJECT

//public:
//    explicit SarStructSettingsModel(const QString str_lastSarSession, QObject *parent = nullptr);
//    ~SarStructSettingsModel() {delete rootItem;}

//    void initialize();
//    // Header:
//    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

//    //bool setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role = Qt::EditRole) override;

//    // Basic functionality:
//    QModelIndex index(int row, int column,
//                      const QModelIndex &parent = QModelIndex()) const override;
//    QModelIndex parent(const QModelIndex &index) const override;

//    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
//    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

////    // Fetch data dynamically:
////    bool hasChildren(const QModelIndex &parent = QModelIndex()) const override;

////    bool canFetchMore(const QModelIndex &parent) const override;
////    void fetchMore(const QModelIndex &parent) override;

//    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

//    // Editable:
//    bool setData(const QModelIndex &index, const QVariant &value,
//                 int role = Qt::EditRole) override;

//    Qt::ItemFlags flags(const QModelIndex& index) const override;

//    SarSessionItem *itemForIndex(const QModelIndex& index) const;
////    void addItem(int row, int column, const QVariant &value);
//    void addItem(const QVector<QVariant> &data, SarSessionItem* parent = nullptr);


//private:
//    //void setUpModelData(const QStringList &lines, SarSessionItem *parent);
//    void setUpModelData(const QString &lines, SarSessionItem *parent);
//    SarSessionItem *itemFromIndex(const QModelIndex& index) const;
//    SarSessionItem *rootItem;

//};

class SarStructSettingsModel : public QAbstractItemModel {
 Q_OBJECT
public:
 SarStructSettingsModel(const QString &data, QObject *parent = 0);
 ~SarStructSettingsModel();
 /*
  Уточняем заголовки методов правильными ключевыми словами C++:
  const - функция не меняет объект, для которого вызывается
  override - функция переопределяет вирутальный метод базового класса
 */
 QVariant data(const QModelIndex &index, int role) const override;
  //получить данные из модельного индекса index с ролью role
 Qt::ItemFlags flags(const QModelIndex &index) const override;
  //получить флаги выбора
 QVariant headerData(int section, Qt::Orientation orientation,
  int role = Qt::DisplayRole) const override;
  //получить данные заголовка
 QModelIndex index(int row, int column,
  const QModelIndex &parent = QModelIndex()) const override;
  //получить модельный индекс по строке и столбцу
 QModelIndex parent(const QModelIndex &index) const override;
  //получить модельный индекс родителя
 int rowCount(const QModelIndex &parent = QModelIndex()) const override;
 int columnCount(const QModelIndex &parent = QModelIndex()) const override;
  //получить количество строк и столбцов для элемента с заданным модельным индексом
 bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
  //установить данные узла с индексом index в значение value
 bool setHeaderData(int section, Qt::Orientation orientation, const QVariant &value,
  int role = Qt::EditRole) override;
  //установить данные заголовка столбца
 bool insertColumns(int position, int columns,const QModelIndex &parent = QModelIndex()) override;
 bool removeColumns(int position, int columns, const QModelIndex &parent = QModelIndex()) override;
 bool insertRows(int position, int rows, const QModelIndex &parent = QModelIndex()) override;
 bool removeRows(int position, int rows, const QModelIndex &parent = QModelIndex()) override;
  //вставка и удаление столбцов и строк
 QVariant value_changed(){return d_value_changed;}

private:
 int setupModelDataLevel(const QString &str, int pos, SarSessionItem *parent);
 void setupModelData(const QString &str, SarSessionItem *parent);
  //внутренний метод для установки данных модели
 SarSessionItem *getItem(const QModelIndex &index) const;
  //внутренний метод для получения элемента
 SarSessionItem *rootItem; //ссылка на корневой узел
 QVariant d_value_changed;
 QList <QModelIndex> d_changedInd;

};

#endif // SARSTRUCTSETTINGSMODEL_H
