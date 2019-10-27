/* ============================================================
* BlockMatrix plugin for Falkon
* Copyright (C) 2019 Sebastian Schmidt
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
* ============================================================ */

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
