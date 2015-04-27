#ifndef QMLTUNERTYPES_H
#define QMLTUNERTYPES_H

#include <QMap>
#include <QMetaType>
#include <QString>

#include <QtDBus>

#ifdef SIGNALSLOT_GRAPH
#  include <QVector>
#endif

struct PropInfo
{
    enum ValueType { Double, Int, Bool, String };

    PropInfo(ValueType valueType=Bool, const QString &name=QString::null);
    ValueType valueType;
    QString name;

    QVariant convert(const QString &value) const;

    static const QVector<PropInfo> &transferedProperties();
};

struct ItemNotation
{
    ItemNotation(const QString &address = QString::null,
                 const QString &parentAddress = QString::null,
                 const QString &type = QString::null,
                 const QMap<QString, QString> &properties = QMap<QString, QString>());

    QString address;
    QString parentAddress;
    QString type;
    QMap<QString, QString> properties;

    bool valid() const { return !address.isEmpty(); }
};

Q_DECLARE_METATYPE(ItemNotation)
Q_DECLARE_METATYPE(QVector<ItemNotation>)

QDBusArgument &operator<<(QDBusArgument &argument, const ItemNotation& itemNotation);
const QDBusArgument &operator>>(const QDBusArgument &argument, ItemNotation &itemNotation);

#ifdef SIGNALSLOT_GRAPH

struct SignalListener
{
    SignalListener(const QByteArray &receiverAddress = QByteArray(),
                   const QByteArray &signalSignature = QByteArray(),
                   int methodType = 0,
                   const QByteArray &methodSignature = QByteArray());

    QByteArray receiverAddress;
    QByteArray signalSignature;
    int methodType;
    QByteArray methodSignature;
};

struct ObjectConnections
{
    QByteArray address;
    QVector<SignalListener> signalsData;
};

Q_DECLARE_METATYPE(SignalListener)
Q_DECLARE_METATYPE(ObjectConnections)
Q_DECLARE_METATYPE(QVector<ObjectConnections>)

QDBusArgument &operator<<(QDBusArgument &argument, const ObjectConnections &objectConnections);
const QDBusArgument &operator>>(const QDBusArgument &argument, ObjectConnections &objectConnections);

#endif // SIGNALSLOT_GRAPH

#endif // QMLTUNERTYPES_H
