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

#include <QDir>
#include <QFile>
#include <QMap>
#include <QString>
#include <QMessageBox>
#include <QTextStream>

#include "blockmatrix_plugin.h"
#include "bm_interceptor.h"
#include "browserwindow.h"
#include "webpage.h"
#include "pluginproxy.h"
#include "mainapplication.h"
#include "tabwidget.h"
#include "webtab.h"
#include "networkmanager.h"

BlockMatrix_Plugin::BlockMatrix_Plugin()
    : QObject()
{
}

void BlockMatrix_Plugin::init(InitState state, const QString &settingsPath)
{
    m_settingsPath = settingsPath;
    m_interceptor = new Interceptor(this);
    QString filepath = QDir(m_settingsPath).filePath("blockmatrix_rules");
    if (!m_interceptor->loadRules(filepath))
    {
        QMessageBox::warning(mApp->getWindow(), "Warning", "Failed to read rules from file", QMessageBox::Ok);
    }

    mApp->networkManager()->installUrlInterceptor(m_interceptor);
}

void BlockMatrix_Plugin::unload()
{
    mApp->networkManager()->removeUrlInterceptor(m_interceptor);
    delete m_interceptor;
}

bool BlockMatrix_Plugin::testPlugin()
{
    return true;
}

