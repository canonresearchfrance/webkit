/*
 * Copyright (C) Canon Inc. 2014
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted, provided that the following conditions
 * are required to be met:
 *
 * 1.  Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 * 2.  Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 * 3.  Neither the name of Canon Inc. nor the names of 
 *     its contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY CANON INC. AND ITS CONTRIBUTORS "AS IS" AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL CANON INC. AND ITS CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "WebNetworkServicesManager.h"

#if ENABLE(DISCOVERY)

#include "WebNetworkService.h"
#include "WebNetworkServicesManagerMessages.h"
#include "WebNetworkServicesManagerProxyMessages.h"
#include "WebPage.h"
#include "WebPageProxyMessages.h"
#include "WebProcess.h"
#include <WebCore/NetworkServiceDescription.h>
#include <WebCore/NetworkServicesClient.h>
#include <WebCore/NetworkServicesController.h>
#include <WebCore/Page.h>

using namespace WebCore;

namespace WebKit {

const char* WebNetworkServicesManager::supplementName()
{
    return "WebNetworkServicesManager";
}

WebNetworkServicesManager::WebNetworkServicesManager(WebProcess* process)
    : m_process(process)
{
    m_process->addMessageReceiver(Messages::WebNetworkServicesManager::messageReceiverName(), *this);
}

WebNetworkServicesManager::~WebNetworkServicesManager()
{
}

void WebNetworkServicesManager::registerWebPage(WebPage* page)
{
    bool wasEmpty = m_pageSet.isEmpty();

    m_pageSet.add(page);

    if (wasEmpty)
        m_process->parentProcessConnection()->send(Messages::WebNetworkServicesManagerProxy::StartUpdating(), 0);
}

void WebNetworkServicesManager::unregisterWebPage(WebPage* page)
{
    m_pageSet.remove(page);

    if (m_pageSet.isEmpty())
        m_process->parentProcessConnection()->send(Messages::WebNetworkServicesManagerProxy::StopUpdating(), 0);
}

void WebNetworkServicesManager::sendDiscoveredServices()
{
    m_process->parentProcessConnection()->send(Messages::WebNetworkServicesManagerProxy::SendDiscoveredServices(), 0);
}

void WebNetworkServicesManager::subscribeEvent(const String& id)
{
    m_process->parentProcessConnection()->send(Messages::WebNetworkServicesManagerProxy::SubscribeEvent(id), 0);
}

void WebNetworkServicesManager::didReceiveNetworkServiceChanged(WebNetworkService& webNetworkService)
{
    NetworkServiceDescription* networkServiceDesc;
    HashSet<WebPage*>::const_iterator it = m_pageSet.begin();
    HashSet<WebPage*>::const_iterator end = m_pageSet.end();

    networkServiceDesc = new NetworkServiceDescription(webNetworkService.id(),
        webNetworkService.name(),
        webNetworkService.type(),
        webNetworkService.url(),
        webNetworkService.config(),
        String());
    networkServiceDesc->setOnline(webNetworkService.online());

    for (; it != end; ++it) {
        WebPage* page = *it;
        if (page->corePage())
            NetworkServicesController::from(page->corePage())->networkServiceChanged(networkServiceDesc);
    }

    delete networkServiceDesc;
}

void WebNetworkServicesManager::didReceiveNetworkServiceEvent(WebNetworkService& webNetworkService, String event)
{
    NetworkServiceDescription* networkServiceDesc;
    HashSet<WebPage*>::const_iterator it = m_pageSet.begin();
    HashSet<WebPage*>::const_iterator end = m_pageSet.end();
    
    networkServiceDesc = new NetworkServiceDescription(webNetworkService.id(),
        webNetworkService.name(),
        webNetworkService.type(),
        webNetworkService.url(),
        webNetworkService.config(),
        String());
    networkServiceDesc->setOnline(webNetworkService.online());

    for (; it != end; ++it) {
        WebPage* page = *it;
        if (page->corePage())
            NetworkServicesController::from(page->corePage())->networkServiceNotifyEvent(networkServiceDesc, event);
    }

    delete networkServiceDesc;
}

void WebNetworkServicesManager::didReceiveDispatchExistingNetworkService(WebNetworkService& webNetworkService)
{
    NetworkServiceDescription* networkServiceDesc;
    HashSet<WebPage*>::const_iterator it = m_pageSet.begin();
    HashSet<WebPage*>::const_iterator end = m_pageSet.end();

    networkServiceDesc = new NetworkServiceDescription(webNetworkService.id(),
        webNetworkService.name(),
        webNetworkService.type(),
        webNetworkService.url(),
        webNetworkService.config(),
        String());
    networkServiceDesc->setOnline(webNetworkService.online());

    for (; it != end; ++it) {
        WebPage* page = *it;
        if (page->corePage())
            NetworkServicesController::from(page->corePage())->networkServiceInitializeRequest(networkServiceDesc);
    }

    delete networkServiceDesc;
}

void WebNetworkServicesManager::didReceiveNotifyDiscoveryFinished()
{
    HashSet<WebPage*>::const_iterator it = m_pageSet.begin();
    HashSet<WebPage*>::const_iterator end = m_pageSet.end();

    for (; it != end; ++it) {
        WebPage* page = *it;
        // Notify the current request if exist 
        bool currentRequest = NetworkServicesController::from(page->corePage())->discoveryFinished();
        
        if (currentRequest) {
            // Notify the client API
            NetworkServicesController::from(page->corePage())->client()->notifyDiscoveryFinished();
        }
    }
}

} // namespace WebKit

#endif // ENABLE(DISCOVERY)
