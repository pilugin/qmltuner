#ifndef QMLTUNERPLUGIN_H
#define QMLTUNERPLUGIN_H

#include <QDeclarativeExtensionPlugin>

class QmlItemManager;

class QmlTunerPlugin : public QDeclarativeExtensionPlugin
{
public:
    QmlTunerPlugin();
    ~QmlTunerPlugin();

    void registerTypes(const char *uri);
    void initializeEngine(QDeclarativeEngine *engine, const char *uri);
private:
};

#endif // QMLTUNERPLUGIN_H
