#include "itempropertiesmodel.h"
#include <QColor>

ItemPropertiesModel::ItemPropertiesModel(QObject *parent) :
    QAbstractTableModel(parent)
{
}

int ItemPropertiesModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return mProperties.size();
}

int ItemPropertiesModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return 2;
}

QModelIndex ItemPropertiesModel::index(int row, int column, const QModelIndex &parent) const
{
    if (parent.isValid() || row >= mProperties.size() || column >= 2 || column<0 || row<0)
        return QModelIndex();
    return createIndex(row, column);
}

QVariant ItemPropertiesModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && (role == Qt::DisplayRole || role == Qt::EditRole)) {
        switch (section) {
        case 0: return tr("Property");
        case 1: return tr("Value");

        default: break;
        }
    }
    return QVariant();
}

QVariant ItemPropertiesModel::data(const QModelIndex &index, int role) const
{
    if (index.isValid()) {
        switch (role) {
        case Qt::DisplayRole:
            if (index.column() == 0)
                return mProperties[index.row()].first;
            else
                return mProperties[index.row()].second;

        case Qt::BackgroundColorRole:
            if (mChangedProperties.contains(qMakePair(mItemNotation.address,
                                                      mProperties[index.row()].first)))
                return QColor(Qt::lightGray);
            else
                return QColor(Qt::white);
        default:
            return QVariant();
        }
    }
    return QVariant();
}


void ItemPropertiesModel::clear()
{
    beginResetModel();
    mProperties.clear();
    endResetModel();
}

void ItemPropertiesModel::updateInfo(const ItemNotation &itemNotation)
{
    clear();

    beginResetModel();
    mItemNotation = itemNotation;
    for (auto itr=itemNotation.properties.begin(); itr!=itemNotation.properties.end(); ++itr)
        mProperties.append(qMakePair(itr.key(), itr.value()));
    endResetModel();
}

void ItemPropertiesModel::propUpdated(const QString &itemAddress, const QString &prop)
{
    mChangedProperties.insert(qMakePair(itemAddress, prop));
}

QModelIndex ItemPropertiesModel::parent(const QModelIndex &child) const
{
    return QModelIndex();
}
