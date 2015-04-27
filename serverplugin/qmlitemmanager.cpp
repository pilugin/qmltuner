#include "qmlitemmanager.h"
#include <QtDeclarative>

QmlItemManager::QmlItemManager(QObject *parent)
    : QmlTunerAdaptor(parent)
{
    mHighlight = new QGraphicsRectItem();
    mHighlight->setPen(QPen("red"));
    mHighlight->setZValue(99999);
}

ItemNotation QmlItemManager::describeItem(const QString &address)
{
    qDebug()<<"[QMLTUNER] QmlItemManager::describeItem()"<<address;

    ItemNotation in;
    if (QDeclarativeItem *di = findQmlItem(address))
        in = itemNotation(di);

    return in;
}

void QmlItemManager::iterateQmlTree(QDeclarativeItem *item, QVector<ItemNotation> &result) const
{
//    qDebug()<<"[QMLTUNER] iterateQmlTree"<<item;

    if (item) {
        ItemNotation in = itemNotation(item);
        result.append(in);

        foreach (QGraphicsItem *child, item->childItems()) {
            if (QDeclarativeItem *childQml = qgraphicsitem_cast<QDeclarativeItem *>(child))
                iterateQmlTree(childQml, result);
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

bool QmlItemManager::setProperty(QDeclarativeItem *di, const QString &property, const QString &value)
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
            return di->setProperty(property.toAscii().data(), itr->convert(value));

        } else if (propPath.size() == 2) { // complex, e.g. anchors.leftMargin
            QVariant v = di->property(propPath.first().toAscii().data());
            if (v.isNull() || !v.isValid()) {
                qDebug()<<"[QMLTUNER] failed to get property "<<propPath.first();
                return false;
            }

            if (QObject *object = variantCast(v)) {
                return object->setProperty(propPath.last().toAscii().data(), itr->convert(value));
            }

        }
        return false;
    }
}

QVector<ItemNotation> QmlItemManager::enumerateItems()
{
    qDebug()<<"[QMLTUNER] QmlItemManager:enumerateItems()";

    QVector<ItemNotation> result;
    foreach (QDeclarativeItem *rootQml, findRootQmlItems())
        iterateQmlTree(rootQml, result);
    return result;
}

bool QmlItemManager::setProperty(const QString &itemAddress, const QString &property, const QString &value)
{
    qDebug()<<"[QMLTUNER] QmlItemManager::setProperty()"<<itemAddress<<property<<value;

    if (QDeclarativeItem *di = findQmlItem(itemAddress))
        return setProperty(di, property, value);

    return false;
}

bool QmlItemManager::highlight(const QString &itemAddress)
{
//    qDebug()<<"[QMLTUNER] QmlItemManager::highlight()"<<itemAddress;
    if (QDeclarativeItem *di = findQmlItem(itemAddress)) {
        if (mHighlight->scene() != di->scene()) {
            if (mHighlight->scene())
                mHighlight->scene()->removeItem(mHighlight);
            if (di->scene())
                di->scene()->addItem(mHighlight);
        }

        if (mHighlight->scene()) {
            QRectF rect = di->boundingRect();
            rect = QRectF(mHighlight->mapFromItem(di, rect.topLeft()), mHighlight->mapFromItem(di, rect.bottomRight()));
            mHighlight->setRect(rect);
        }

        return true;
    }

    return false;
}

bool QmlItemManager::showTarget(const QString &pathToFile, double opacity)
{
    foreach (QDeclarativeView *dv, findQmlViews()) {
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
}

QVector<QDeclarativeView *> QmlItemManager::findQmlViews() const
{
    QVector<QDeclarativeView *> views;
    foreach (QWidget *w, qApp->allWidgets()) {
        if (QDeclarativeView *view = qobject_cast<QDeclarativeView *>(w))
            views.append(view);
    }
    return views;
}

QVector<QDeclarativeItem *> QmlItemManager::findRootQmlItems() const
{
    QVector<QDeclarativeItem *> items;
    foreach (QDeclarativeView *view, findQmlViews()) {
        if (QDeclarativeItem *rootItem = qgraphicsitem_cast<QDeclarativeItem *>(view->rootObject()))
            items.append(rootItem);
    }
    return items;
}

QDeclarativeItem *QmlItemManager::findQmlItem(const QString &address, QDeclarativeItem *root) const
{
    if (!root) {
        foreach (root, findRootQmlItems()) {
            if (addressString(root) == address)
                return root;

            if (QDeclarativeItem *result = findQmlItem(address, root))
                return result;
        }
    } else {
        if (addressString(root) == address)
            return root;

        foreach (QGraphicsItem *child, root->childItems()) {
            if (QDeclarativeItem *childQml = qgraphicsitem_cast<QDeclarativeItem *>(child)) {
                if (QDeclarativeItem *result = findQmlItem(address, childQml))
                    return result;
            }
        }
    }

    return 0;
}


QMap<QString, QString> QmlItemManager::getPropertyMap(QDeclarativeItem *item) const
{
    QMap<QString, QString> result;

    foreach (const PropInfo &pi, PropInfo::transferedProperties()) {
        QStringList propPath = pi.name.split(".", QString::KeepEmptyParts);
        if (propPath.size() == 1) { // simple case
            result.insert(pi.name, item->property(pi.name.toAscii().data()).toString());

        } else if (propPath.size() == 2) { // complex, e.g. anchors.leftMargin
            QVariant v = item->property(propPath.first().toAscii().data());
            if (v.isNull() || !v.isValid()) {
                qDebug()<<"[QMLTUNER] failed to get property "<<propPath.first();
                continue;
            }

            if (QObject *object = variantCast(v)) {
                result.insert(pi.name, object->property(propPath.last().toAscii().data()).toString());

            } else {
                qDebug()<<"[QMLTUNER] casting failed" << propPath.first();
                continue;
            }

        }
    }

    return result;
}

ItemNotation QmlItemManager::itemNotation(QDeclarativeItem *item) const
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

bool QmlItemManager::checkItemNeeded(QDeclarativeItem *item) const
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
