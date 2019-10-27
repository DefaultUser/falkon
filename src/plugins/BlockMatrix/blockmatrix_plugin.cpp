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

