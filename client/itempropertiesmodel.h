#ifndef ITEMPROPERTIESMODEL_H
#define ITEMPROPERTIESMODEL_H

#include <QAbstractTableModel>
#include "qmltunertypes.h"
#include <QVector>
#include <QPair>
#include <QMap>
#include <QSet>

class ItemPropertiesModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    explicit ItemPropertiesModel(QObject *parent = 0);

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QModelIndex index(int row, int column, const QModelIndex &parent) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    QVariant data(const QModelIndex &index, int role) const;
    QModelIndex parent(const QModelIndex &child) const;
    void clear();

    void updateInfo(const ItemNotation &itemNotation);

    void propUpdated(const QString &itemAddress, const QString &prop);

private:
    QVector<QPair<QString, QString>> mProperties;
    ItemNotation mItemNotation;

    QSet<QPair<QString, QString> > mChangedProperties;
};

#endif // ITEMPROPERTIESMODEL_H
