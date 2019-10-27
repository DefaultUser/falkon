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
