#ifndef QMLTUNERPLUGIN_H
#define QMLTUNERPLUGIN_H

#include <QQmlExtensionPlugin>

class QmlItemManager;

class QmlTunerPlugin : public QQmlExtensionPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.pilugin.QmlTuner" FILE "qmltuner.json")
public:
    QmlTunerPlugin();
    ~QmlTunerPlugin();

    void registerTypes(const char *uri);
    void initializeEngine(QQmlEngine *engine, const char *uri);
private:
};

#endif // QMLTUNERPLUGIN_H
