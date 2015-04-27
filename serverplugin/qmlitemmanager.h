#ifndef QMLITEMMANAGER_H
#define QMLITEMMANAGER_H

#include "qmltuneradaptor.h"

class QDeclarativeItem;
class QDeclarativeView;

class QGraphicsRectItem;
class QGraphicsPixmapItem;

class QmlItemManager : public QmlTunerAdaptor
{
    Q_OBJECT
public:
    explicit QmlItemManager(QObject *parent = 0);

signals:

public slots:
    ItemNotation describeItem(const QString &address);
    QVector<ItemNotation> enumerateItems();
    bool setProperty(const QString &itemAddress, const QString &property, const QString &value);
    bool highlight(const QString &itemAddress);
    bool showTarget(const QString &pathToFile, double opacity);

private:
    QVector<QDeclarativeView *> findQmlViews() const;
    QVector<QDeclarativeItem *> findRootQmlItems() const;
    QDeclarativeItem *findQmlItem(const QString &address, QDeclarativeItem *root = 0) const;

    QMap<QString, QString> getPropertyMap(QDeclarativeItem *item) const;
    ItemNotation itemNotation(QDeclarativeItem *item) const;

    bool checkItemNeeded(QDeclarativeItem *item) const;

    QString addressString(void *address) const;
    void iterateQmlTree(QDeclarativeItem *item, QVector<ItemNotation> &result) const;

    QObject *variantCast(const QVariant &variant) const;

    bool setProperty(QDeclarativeItem *di, const QString &property, const QString &value);

private:
    QStringList mIgnoreTypes;

    QGraphicsRectItem *mHighlight;

    QHash<QDeclarativeView *, QGraphicsPixmapItem *> mTargets;
};

#endif // QMLITEMMANAGER_H
