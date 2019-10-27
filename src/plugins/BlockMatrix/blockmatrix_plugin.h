#ifndef BLOCKMATRIX_PLUGIN_H
#define BLOCKMATRIX_PLUGIN_H

#include <QMap>

#include "plugininterface.h"
#include "bm_interceptor.h"

class WebPage;

class BlockMatrix_Plugin : public QObject, public PluginInterface
{
    Q_OBJECT
    Q_INTERFACES(PluginInterface)
    Q_PLUGIN_METADATA(IID "Falkon.Browser.plugin.BlockMatrix" FILE "blockmatrix.json")

public:
    explicit BlockMatrix_Plugin();
    void init(InitState state, const QString &settingsPath) override;
    void unload() override;
    bool testPlugin() override;

private:
    Interceptor* m_interceptor;
    QString m_settingsPath;
};

#endif // BLOCKMATRIX_PLUGIN_H
