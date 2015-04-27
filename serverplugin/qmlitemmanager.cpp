#include "qmlitemmanager.h"
#include <QtWidgets>
#include <QtQml>
#include <QtQuick>

QmlItemManager::QmlItemManager(QObject *parent)
    : QmlTunerAdaptor(parent)
{
    mHighlight = nullptr;
    mHighlightFactory = nullptr;
}

ItemNotation QmlItemManager::describeItem(const QString &address)
{
    qDebug()<<"[QMLTUNER] QmlItemManager::describeItem()"<<address;

    ItemNotation in;
    if (QQuickItem *di = findQmlItem(address))
        in = itemNotation(di);

    return in;
}

void QmlItemManager::iterateQmlTree(QQuickItem *item, QVector<ItemNotation> &result) const
{
//    qDebug()<<"[QMLTUNER] iterateQmlTree"<<item;

    if (item && item != mHighlight) {
        ItemNotation in = itemNotation(item);
        result.append(in);

        foreach (QQuickItem *child, item->childItems()) {
            iterateQmlTree(child, result);
        }
    }
}

QObject *QmlItemManager::variantCast(const QVariant &variant) const
{
    if (variant.isNull())
        return 0;

    QVariant::DataPtr &variantData = const_cast<QVariant &>(variant).data_ptr();
    QObject *obj = 0;
    if (variantData.is_shared)
        obj = (QObject *)*reinterpret_cast<void **>(variantData.data.shared->ptr);
    else
        obj = variantData.data.o;

    return obj;
}

bool QmlItemManager::setProperty(QQuickItem *di, const QString &property, const QString &value)
{
    auto itr = std::find_if(PropInfo::transferedProperties().begin(),
                            PropInfo::transferedProperties().end(),
                            [property](const PropInfo &pi) { return pi.name == property; }
    );
    if (itr == PropInfo::transferedProperties().end()) {
        qDebug()<<"[QMLTUNER] failed to find property"<<property;
        return false;

    } else {
        qDebug()<<"[QMLTUNER] setting "<<itr->name;

        QStringList propPath = itr->name.split(".", QString::KeepEmptyParts);
        if (propPath.size() == 1) { // simple case
            return di->setProperty(property.toLatin1().data(), itr->convert(value));

        } else if (propPath.size() == 2) { // complex, e.g. anchors.leftMargin
            QVariant v = di->property(propPath.first().toLatin1().data());
            if (v.isNull() || !v.isValid()) {
                qDebug()<<"[QMLTUNER] failed to get property "<<propPath.first();
                return false;
            }

            if (QObject *object = variantCast(v)) {
                return object->setProperty(propPath.last().toLatin1().data(), itr->convert(value));
            }

        }
        return false;
    }
}

QVector<ItemNotation> QmlItemManager::enumerateItems()
{
    qDebug()<<"[QMLTUNER] QmlItemManager:enumerateItems()";

    QVector<ItemNotation> result;
    foreach (QQuickItem *rootQml, findRootQmlItems())
        iterateQmlTree(rootQml, result);
    return result;
}

bool QmlItemManager::setProperty(const QString &itemAddress, const QString &property, const QString &value)
{
    qDebug()<<"[QMLTUNER] QmlItemManager::setProperty()"<<itemAddress<<property<<value;

    if (QQuickItem *di = findQmlItem(itemAddress))
        return setProperty(di, property, value);

    return false;
}

bool QmlItemManager::highlight(const QString &itemAddress)
{
    qDebug()<<"[QMLTUNER] QmlItemManager::highlight()"<<itemAddress;

    if (QQuickItem *di = findQmlItem(itemAddress)) {
        if (di == di->window()->contentItem())
            return false; // cannot highlight root

        // create component, if needed
        if (!mHighlightFactory || qmlEngine(di) != qmlEngine(mHighlightFactory) ) {
            if (mHighlightFactory)
                mHighlightFactory->deleteLater();
            mHighlightFactory = new QQmlComponent( qmlEngine(di), this);
            mHighlightFactory->setData(
                        "import QtQuick 2.0\n"
                        "Rectangle { border.color: \"red\"\n"
                        "color: \"#00000000\"\n"
                        "}"
                        , QUrl("Highlight") );

            if (mHighlightFactory->isError()) {
                qDebug()<<mHighlightFactory->errors();
                return false;
            }
        }

        // create item, if needed
        if (!mHighlight || qmlEngine(di) != qmlEngine(mHighlight) ) {
            if (mHighlight)
                mHighlight->deleteLater();

            mHighlight = qobject_cast<QQuickItem *>(mHighlightFactory->create());
        }

        //qDebug()<<mHighlight;

        if (mHighlight) {
            mHighlight->setParentItem( di->window()->contentItem() );
            QRectF rect = di->boundingRect();

            //qDebug()<<rect;

            mHighlight->setX( rect.x() );
            mHighlight->setY( rect.y() );
            mHighlight->setWidth( rect.width() );
            mHighlight->setHeight( rect.height() );

            return true;
        }
    }
    return false;
}

bool QmlItemManager::showTarget(const QString &pathToFile, double opacity)
{
#if 0
    foreach (QQuickView *dv, findQmlViews()) {
        QGraphicsPixmapItem *overlayItem = mTargets.value(dv);
        if (!overlayItem) {
            overlayItem = dv->scene()->addPixmap(QPixmap());
            overlayItem->setPos(0, 0);
            overlayItem->setZValue(99998);
            mTargets.insert(dv, overlayItem);
        }

        if (pathToFile != overlayItem->data(Qt::UserRole).toString()) {
            QPixmap pixmap(pathToFile);
            overlayItem->setPixmap(pixmap);
            overlayItem->setData(Qt::UserRole, pathToFile);
        }
        overlayItem->setOpacity(opacity);
    }

    return !pathToFile.isEmpty(); // ### what for?
#else
    return true;
#endif
}

QVector<QQuickWindow *> QmlItemManager::findQmlViews() const
{
    QVector<QQuickWindow *> views;
    foreach (QWindow *w, qApp->topLevelWindows()) {
        if (QQuickWindow *view = qobject_cast<QQuickWindow *>(w))
            views.append(view);
    }
    return views;
}

QVector<QQuickItem *> QmlItemManager::findRootQmlItems() const
{    
    QVector<QQuickItem *> items;
    foreach (QQuickWindow *view, findQmlViews()) {
        if (QQuickItem *rootItem = view->contentItem())
            items.append(rootItem);
    }
    return items;
}

QQuickItem *QmlItemManager::findQmlItem(const QString &address, QQuickItem *root) const
{
    if (!root) {
        foreach (root, findRootQmlItems()) {
            if (addressString(root) == address)
                return root;

            if (QQuickItem *result = findQmlItem(address, root))
                return result;
        }
    } else {
        if (addressString(root) == address)
            return root;

        foreach (QQuickItem *child, root->childItems()) {
            if (QQuickItem *result = findQmlItem(address, child))
                return result;
        }
    }

    return 0;
}


QMap<QString, QString> QmlItemManager::getPropertyMap(QQuickItem *item) const
{
    QMap<QString, QString> result;

    foreach (const PropInfo &pi, PropInfo::transferedProperties()) {
        QStringList propPath = pi.name.split(".", QString::KeepEmptyParts);
        if (propPath.size() == 1) { // simple case
            result.insert(pi.name, item->property(pi.name.toLatin1().data()).toString());

        } else if (propPath.size() == 2) { // complex, e.g. anchors.leftMargin
            QVariant v = item->property(propPath.first().toLatin1().data());
            if (v.isNull() || !v.isValid()) {
                qDebug()<<"[QMLTUNER] failed to get property "<<propPath.first();
                continue;
            }

            if (QObject *object = variantCast(v)) {
                result.insert(pi.name, object->property(propPath.last().toLatin1().data()).toString());

            } else {
                qDebug()<<"[QMLTUNER] casting failed" << propPath.first();
                continue;
            }

        }
    }

    return result;
}

ItemNotation QmlItemManager::itemNotation(QQuickItem *item) const
{
    ItemNotation in;
    if (item) {
        in.address = addressString(item);
        in.parentAddress = addressString(item->parentItem());
        in.type = item->metaObject()->className();
        in.properties = getPropertyMap(item);
    }
    return in;
}

bool QmlItemManager::checkItemNeeded(QQuickItem *item) const
{
    foreach (const QString &str, mIgnoreTypes)
        if (QString(item->metaObject()->className()).contains(str)) {
            return false;
        }

    return true;
}

QString QmlItemManager::addressString(void *address) const
{
    return QString().sprintf("%p", address);
}
