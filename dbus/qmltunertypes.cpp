#include "qmltunertypes.h"

#include <QtDBus>

class RegisterMetaTypes
{
public:
    inline RegisterMetaTypes()
    {
        qRegisterMetaType<ItemNotation>();
        qRegisterMetaType<QVector<ItemNotation> >();

        qDBusRegisterMetaType<ItemNotation>();
        qDBusRegisterMetaType<QVector<ItemNotation> >();

#ifdef SIGNALSLOT_GRAPH
        qRegisterMetaType<ObjectConnections>();
        qRegisterMetaType<QVector<ObjectConnections>>();

        qDBusRegisterMetaType<ObjectConnections>();
        qDBusRegisterMetaType<QVector<ObjectConnections>>();
#endif
    }
} _registerMetaTypes;


ItemNotation::ItemNotation(const QString &address_,
                           const QString &parentAddress_,
                           const QString &type_,
                           const QMap<QString, QString> &properties_) :
    address(address_),
    parentAddress(parentAddress_),
    type(type_),
    properties(properties_)
{
}

const QDBusArgument &operator>>(const QDBusArgument &argument, ItemNotation &itemNotation)
{
    argument.beginStructure();
    argument >> itemNotation.address;
    argument >> itemNotation.parentAddress;
    argument >> itemNotation.type;
    argument.beginMap();
    while (!argument.atEnd()) {
        QString key, value;
        argument.beginMapEntry();
        argument >> key >> value;
        argument.endMapEntry();
        itemNotation.properties.insert(key, value);
    }
    argument.endMap();
    argument.endStructure();

//    qDebug()<<"[QMLTUNER] >> "<< itemNotation.address
//              << itemNotation.parentAddress
//                 << itemNotation.type
//                 << itemNotation.properties;

    return argument;
}

QDBusArgument &operator<<(QDBusArgument &argument, const ItemNotation& itemNotation)
{
    argument.beginStructure();
    argument << itemNotation.address;
    argument << itemNotation.parentAddress;
    argument << itemNotation.type;
    argument.beginMap(QVariant::String, QVariant::String);
    for (auto itr=itemNotation.properties.begin(); itr!=itemNotation.properties.end(); ++itr) {
        argument.beginMapEntry();
        argument << itr.key() << itr.value();
        argument.endMapEntry();
    }
    argument.endMap();
    argument.endStructure();

//    qDebug()<<"[QMLTUNER] << "<< itemNotation.address
//              << itemNotation.parentAddress
//                 << itemNotation.type
//                 << itemNotation.properties;

    return argument;
}


PropInfo::PropInfo(PropInfo::ValueType valueType_, const QString &name_) :
    valueType(valueType_),
    name(name_)
{
}

QVariant PropInfo::convert(const QString &value) const
{
    switch (valueType) {
    case Bool:
        return value=="true";
    case Int:
        return value.toInt();
    case Double:
        return value.toDouble();
    default:
        return value;
    }
}

const QVector<PropInfo> &PropInfo::transferedProperties()
{
    class TransferedProperties : public QVector<PropInfo>
    {
    public:
        TransferedProperties()
        {
            append(PropInfo(Double, "x"));
            append(PropInfo(Double, "y"));
            append(PropInfo(Double, "z"));
            append(PropInfo(Double, "width"));
            append(PropInfo(Double, "height"));
            append(PropInfo(Double, "anchors.leftMargin"));
            append(PropInfo(Double, "anchors.rightMargin"));
            append(PropInfo(Double, "anchors.topMargin"));
            append(PropInfo(Double, "anchors.bottomMargin"));
            append(PropInfo(Double, "anchors.horizontalCenterOffset"));
            append(PropInfo(Double, "anchors.verticalCenterOffset"));
            append(PropInfo(Double, "opacity"));

            append(PropInfo(Bool, "visible"));
            append(PropInfo(Bool, "clip"));
            append(PropInfo(Bool, "enabled"));

            append(PropInfo(String, "objectName"));
            append(PropInfo(String, "color"));
            append(PropInfo(String, "font"));
            append(PropInfo(String, "text"));
            append(PropInfo(String, "source"));

//            append(PropInfo(Double, "font.letterSpacing"));
//            append(PropInfo(Double, "font.wordSpacing"));
        }
    };
    static const TransferedProperties sTransferedProperties;

    return sTransferedProperties;
}


#ifdef SIGNALSLOT_GRAPH

SignalListener::SignalListener(const QByteArray &receiverAddress_,
                               const QByteArray &signalSignature_,
                               int methodType_,
                               const QByteArray &methodSignature_)
    : receiverAddress(receiverAddress_),
      signalSignature(signalSignature_),
      methodType(methodType_),
      methodSignature(methodSignature_)
{
}


QDBusArgument &operator<<(QDBusArgument &argument, const ObjectConnections &objectConnections)
{
    argument.beginStructure();
    argument << objectConnections.address;
    argument.beginArray(qMetaTypeId<SignalListener>());
    foreach (const SignalListener &sl, objectConnections.signalsData) {
        argument.beginStructure();
        argument << sl.receiverAddress << sl.signalSignature << sl.methodType << sl.methodSignature;
        argument.endStructure();
    }
    argument.endArray();
    argument.endStructure();

    return argument;
}

const QDBusArgument &operator>>(const QDBusArgument &argument, ObjectConnections &objectConnections)
{
    argument.beginStructure();
    argument >> objectConnections.address;
    argument.beginArray();
    while (!argument.atEnd()) {
        argument.beginStructure();
        SignalListener sl;
        argument >> sl.receiverAddress >> sl.signalSignature >> sl.methodType >> sl.methodSignature;
        argument.endStructure();

        objectConnections.signalsData.append(sl);
    }
    argument.endArray();
    argument.endStructure();

    return argument;
}

#endif // SIGNALSLOT_GRAPH
