#ifndef QMLITEMSTREE_H
#define QMLITEMSTREE_H

#include <QAbstractItemModel>
#include "qmltunertypes.h"
#include <QSet>

class QmlItemsTree : public QAbstractItemModel
{
    Q_OBJECT
public:
    explicit QmlItemsTree(QObject *parent = 0);
    
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QModelIndex index(int row, int column, const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    QModelIndex parent(const QModelIndex &child) const;
    void clear();

    void updateInfo(const QVector<ItemNotation> &itemNotations);
    void itemUpdated(const QString &address);

    ItemNotation itemNotation(const QModelIndex &index) const;
signals:
    
public slots:

private:
    struct Node {
        Node(Node *parent=0, const ItemNotation &data=ItemNotation());

        Node *parent;
        QVector<Node *> children;

        ItemNotation data;
    };

    Node mRootNode;
    QMap<QString, Node *> mAllNodes;

    QSet<QString> mChangedItems;
};

#endif // QMLITEMSTREE_H
