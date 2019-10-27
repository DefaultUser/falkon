#include "bm_interceptor.h"
#include <QDebug>
#include <QByteArray>
#include <QFile>

#define BLOCKMATRIX_DEBUG

ResourceCategory resourceTypeToCategory(QWebEngineUrlRequestInfo::ResourceType type)
{
    switch (type)
    {
        case QWebEngineUrlRequestInfo::ResourceTypeScript:
            return ResourceCategory::SCRIPT;
        case QWebEngineUrlRequestInfo::ResourceTypeImage:
            return ResourceCategory::IMAGE;
        case QWebEngineUrlRequestInfo::ResourceTypeMedia:
            return ResourceCategory::MEDIA;
        case QWebEngineUrlRequestInfo::ResourceTypeStylesheet:
            return ResourceCategory::CSS;
        case QWebEngineUrlRequestInfo::ResourceTypeXhr:
            return ResourceCategory::XHR;
        case QWebEngineUrlRequestInfo::ResourceTypeSubFrame:
        case QWebEngineUrlRequestInfo::ResourceTypeObject:
            return ResourceCategory::FRAME;
        default:
            return ResourceCategory::OTHER;
    }
}

Interceptor::Interceptor(QObject* parent): UrlInterceptor(parent)
{
}

bool Interceptor::shouldBlock(QUrl firstPartyUrl, QUrl requestUrl,
                              QWebEngineUrlRequestInfo::ResourceType resourceType)
{
    ResourceCategory resourceCategory = resourceTypeToCategory(resourceType);
    QString firstPartyHost = firstPartyUrl.host();
    QString requestUrlHost = requestUrl.host();
    bool isFirstParty = (firstPartyHost == requestUrlHost);
#ifdef BLOCKMATRIX_DEBUG
    qDebug() << requestUrl << " from " << firstPartyUrl;
#endif
    for (auto firstPartyRuleString : QStringList({firstPartyHost, "*"}))
    {
        if (m_rules.contains(firstPartyRuleString))
        {
            auto firstPartyRule = m_rules[firstPartyRuleString];
            QStringList requestRulePool = QStringList({requestUrlHost, "*"});
            // TODO: add main site for subdomains
            if (isFirstParty)
            {
                requestRulePool.insert(1, "1st-party");
            }
            for (auto requestRuleString : requestRulePool)
            {
                if (firstPartyRule.contains(requestRuleString))
                {
                    auto requestRule = firstPartyRule[requestRuleString];
                    for (auto category : QList<ResourceCategory>(
                                {resourceCategory, ResourceCategory::ALL}))
                    {
                        if (requestRule.contains(category))
                        {
                            if (requestRule[category] == InterceptorRuleBehaviour::INHERIT)
                            {
                                break;
                            }
                            return (requestRule[category] == InterceptorRuleBehaviour::BLOCK);
                        }
                    }
                }
            }
        }
    }
    return true;
}

void Interceptor::addRule(QString firstPartyHost, QString requestUrlHost,
                          ResourceCategory category, InterceptorRuleBehaviour behaviour)
{
    if (!m_rules.contains(firstPartyHost))
    {
        m_rules[firstPartyHost] = QMap<QString, QMap<ResourceCategory, InterceptorRuleBehaviour>>();
    }
    if (!m_rules[firstPartyHost].contains(requestUrlHost))
    {
        m_rules[firstPartyHost][requestUrlHost] = QMap<ResourceCategory, InterceptorRuleBehaviour>();
    }
    m_rules[firstPartyHost][requestUrlHost][category] = behaviour;
#ifdef BLOCKMATRIX_DEBUG
    qDebug() << "Rule : " << firstPartyHost << " " << requestUrlHost << " " << (int)category << " " << (int)behaviour;
#endif
}

void Interceptor::defaultRules()
{
    m_rules = QMap<QString, QMap<QString, QMap<ResourceCategory, InterceptorRuleBehaviour>>>();
    addRule("*", "*", ResourceCategory::ALL, InterceptorRuleBehaviour::BLOCK);
    addRule("*", "1st-party", ResourceCategory::ALL, InterceptorRuleBehaviour::ALLOW);
    addRule("*", "1st-party", ResourceCategory::SCRIPT, InterceptorRuleBehaviour::BLOCK);
}

bool Interceptor::loadRules(QString &filepath)
{
    defaultRules();
    QFile f(filepath);
    if (!f.exists())
    {
        qDebug() << "Rules file not found";
        return false;
    }
    if (!f.open(QIODevice::ReadOnly))
    {
        qDebug() << "Could not open rules file";
        return false;
    }
    bool rulesContainErrors = false;
    QTextStream sstream(&f);
    QString line;
    while (sstream.readLineInto(&line))
    {
        QStringList rule = line.split(" ");
        if (rule.size() != 4)
        {
            rulesContainErrors = true;
            continue;
        }
        ResourceCategory category;
        if (rule[2] == "script")
        {
            category = ResourceCategory::SCRIPT;
        }
        else if (rule[2] == "image")
        {
            category = ResourceCategory::IMAGE;
        }
        else if (rule[2] == "media")
        {
            category = ResourceCategory::MEDIA;
        }
        else if (rule[2] == "css")
        {
            category = ResourceCategory::CSS;
        }
        else if (rule[2] == "xhr")
        {
            category = ResourceCategory::XHR;
        }
        else if (rule[2] == "frame")
        {
            category = ResourceCategory::FRAME;
        }
        else if (rule[2] == "other")
        {
            category = ResourceCategory::OTHER;
        }
        else if (rule[2] == "*")
        {
            category = ResourceCategory::ALL;
        }
        else
        {
            rulesContainErrors = true;
            continue;
        }
        InterceptorRuleBehaviour behaviour;
        if (rule[3] == "block")
        {
            behaviour = InterceptorRuleBehaviour::BLOCK;
        }
        else if (rule[3] == "allow")
        {
            behaviour = InterceptorRuleBehaviour::ALLOW;
        }
        else if (rule[3] == "inherit")
        {
            behaviour = InterceptorRuleBehaviour::INHERIT;
        }
        else
        {
            rulesContainErrors = true;
            continue;
        }
        addRule(rule[0], rule[1], category, behaviour);
    }
    if (rulesContainErrors)
    {
        qDebug() << "Warning, rules contain errors, dropping those rules";
    }
    return true;
}

void Interceptor::interceptRequest(QWebEngineUrlRequestInfo &request)
{
    QUrl firstPartyUrl = request.firstPartyUrl();
    QUrl requestUrl = request.requestUrl();
    QWebEngineUrlRequestInfo::ResourceType resourceType = request.resourceType();
    // Only block for http(s)
    QString scheme = firstPartyUrl.scheme();
    if (!(scheme == "https" || scheme == "http"))
    {
        return;
    }
    if (resourceType == QWebEngineUrlRequestInfo::ResourceTypeMainFrame)
    {
        // Main frame should never be blocked
        return;
    }
    if (resourceType == QWebEngineUrlRequestInfo::ResourceTypeFavicon)
    {
        // Favicon should never be blocked
        return;
    }
    bool isBlocked = shouldBlock(firstPartyUrl, requestUrl, resourceType);
    if (isBlocked)
    {
#ifdef BLOCKMATRIX_DEBUG
        qDebug() << "Blocking request to " << requestUrl << " from " << firstPartyUrl;
#endif
        request.block(true);
    }
    BlockMatrix_EncounteredRequest r;
    r.firstPartyHost = firstPartyUrl.host();
    r.requestUrlHost = requestUrl.host();
    r.category = resourceTypeToCategory(resourceType);
    r.isBlocked = isBlocked;
    emit requestEncountered(r);
}

