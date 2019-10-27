#ifndef BM_INTERCEPTOR_H
#define BM_INTERCEPTOR_H

#include <QMap>

#include "urlinterceptor.h"

enum class InterceptorRuleBehaviour : uint8_t {BLOCK, ALLOW, INHERIT};
enum class ResourceCategory {SCRIPT, IMAGE, MEDIA, CSS, XHR, FRAME, OTHER, ALL=255};

typedef struct {
    QString firstPartyHost;
    QString requestUrlHost;
    ResourceCategory category;
    bool isBlocked;
} BlockMatrix_EncounteredRequest;


class Interceptor : public UrlInterceptor
{
    Q_OBJECT
public:
    explicit Interceptor(QObject* parent);
    void interceptRequest(QWebEngineUrlRequestInfo &request) override;
    void addRule(QString firstPartyHost, QString requestUrlHost, ResourceCategory category,
                 InterceptorRuleBehaviour behaviour);
    void defaultRules();
    bool loadRules(QString &filepath);

Q_SIGNALS:
    void requestEncountered(const BlockMatrix_EncounteredRequest &request);

private:
    bool shouldBlock(QUrl firstParty, QUrl requestUrl,
                     QWebEngineUrlRequestInfo::ResourceType resourceType);
    QMap<QString, QMap<QString, QMap<ResourceCategory, InterceptorRuleBehaviour>>> m_rules;
};

#endif // BM_INTERCEPTOR_H
