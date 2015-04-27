#ifndef QMLITEMMANAGER_H
#define QMLITEMMANAGER_H

#include "qmltuneradaptor.h"

class QQuickItem;
class QQuickWindow;
class QQmlComponent;

class QWindow;

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
    QVector<QQuickWindow *> findQmlViews() const;
    void findQmlViews(QVector<QQuickWindow *> &qmls, QWindow *parent) const;
    QVector<QQuickItem *> findRootQmlItems() const;
    QQuickItem *findQmlItem(const QString &address, QQuickItem *root = 0) const;

    QMap<QString, QString> getPropertyMap(QQuickItem *item) const;
    ItemNotation itemNotation(QQuickItem *item) const;

    bool checkItemNeeded(QQuickItem *item) const;

    QString addressString(void *address) const;
    void iterateQmlTree(QQuickItem *item, QVector<ItemNotation> &result) const;

    QObject *variantCast(const QVariant &variant) const;

    bool setProperty(QQuickItem *di, const QString &property, const QString &value);

private:
    QStringList mIgnoreTypes;

    QQuickItem *mHighlight;
    QQmlComponent *mHighlightFactory;

    //QHash<QQuickWindow *, QGraphicsPixmapItem *> mTargets;
};

#endif // QMLITEMMANAGER_H
