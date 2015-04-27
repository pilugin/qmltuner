#include "qmlitemstree.h"
#include <QColor>

QmlItemsTree::QmlItemsTree(QObject *parent) :
    QAbstractItemModel(parent)
{
}

int QmlItemsTree::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid()) {
        Node *node = static_cast<Node *>(parent.internalPointer());

//        qDebug()<<"rowCount "        << (node ? node->children.size() : 0);

        return node ? node->children.size() : 0;
    } else {
        return mRootNode.children.size();
    }
}

int QmlItemsTree::columnCount(const QModelIndex &parent) const
{
    return 1;
}

QModelIndex QmlItemsTree::index(int row, int column, const QModelIndex &parent) const
{
    if (column != 0)
        return QModelIndex();

    if (parent.isValid()) {
        if (Node *node = static_cast<Node *>(parent.internalPointer())) {
            if (row >= node->children.size())
                return QModelIndex();
            else
                return createIndex(row, column, node->children[row]);
        } else
            return QModelIndex();

    } else {
        if (row >= mRootNode.children.size())
            return QModelIndex();
        else
            return createIndex(row, column, mRootNode.children[row]);
    }
}

QVariant QmlItemsTree::data(const QModelIndex &index, int role) const
{
    Node *node = static_cast<Node *>(index.internalPointer());
    if (index.isValid() && node) {

//        qDebug()<<"DATA"        <<index.row()<<role;

        switch (role) {
        case Qt::DisplayRole:
            return node->data.type;

        case Qt::BackgroundColorRole:
            if (mChangedItems.contains(node->data.address))
                return QColor(Qt::lightGray);
            else
                return QColor(Qt::white);
        default:
            return QVariant();
            //return QString::number(role);
        }
    } else
        return QVariant();
}

QModelIndex QmlItemsTree::parent(const QModelIndex &child) const
{
    Node *node = static_cast<Node *>(child.internalPointer());
    if (child.isValid() && node && node->parent) {
//        return QModelIndex();
        return createIndex(0, 1, node->parent);

    } else {
        return QModelIndex();
    }
}

void QmlItemsTree::clear()
{
    beginResetModel();
    qDeleteAll(mAllNodes);
    mAllNodes.clear();
    mRootNode.children.clear();
    endResetModel();
}

void QmlItemsTree::updateInfo(const QVector<ItemNotation> &itemNotations)
{
    clear();

    beginResetModel();
    foreach (const ItemNotation &in, itemNotations) {
        auto itr = mAllNodes.find(in.parentAddress);
        Node *parentNode = (itr == mAllNodes.end()) ? &mRootNode : *itr;
        Node *newNode = new Node(parentNode, in);

        parentNode->children.append(newNode);
        mAllNodes.insert(in.address, newNode);
    }
    endResetModel();
}

void QmlItemsTree::itemUpdated(const QString &address)
{
    mChangedItems.insert(address);
}

ItemNotation QmlItemsTree::itemNotation(const QModelIndex &index) const
{
    Node *node = static_cast<Node *>(index.internalPointer());
    if (index.isValid() && node)
        return node->data;
    return ItemNotation();
}


QmlItemsTree::Node::Node(QmlItemsTree::Node *parent_, const ItemNotation &data_) :
    parent(parent_), data(data_)
{
}
