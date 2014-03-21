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
#include "WebNetworkServicesManagerProxy.h"

#if ENABLE(DISCOVERY)

#include "WebContext.h"
#include "WebNetworkService.h"
#include "WebNetworkServicesManagerMessages.h"
#include "WebNetworkServicesManagerProxyMessages.h"

namespace WebKit {

const char* WebNetworkServicesManagerProxy::supplementName()
{
    return "WebNetworkServicesManagerProxy";
}  

PassRefPtr<WebNetworkServicesManagerProxy> WebNetworkServicesManagerProxy::create(WebContext* context)
{
    return adoptRef(new WebNetworkServicesManagerProxy(context));
}

WebNetworkServicesManagerProxy::WebNetworkServicesManagerProxy(WebContext *context)
    : WebContextSupplement(context)
    , m_isUpdating(false)
{
    WebContextSupplement::context()->addMessageReceiver(Messages::WebNetworkServicesManagerProxy::messageReceiverName(), *this);
}

WebNetworkServicesManagerProxy::~WebNetworkServicesManagerProxy()
{
}

void WebNetworkServicesManagerProxy::initializeProvider(const WKNetworkServicesProviderBase* provider)
{
    m_provider.initialize(provider);
}

void WebNetworkServicesManagerProxy::didReceiveNetworkServiceChanged(WebKit::WebNetworkService* networkService)
{
    if (!context())
        return;

    context()->sendToAllProcesses(Messages::WebNetworkServicesManager::DidReceiveNetworkServiceChanged(*networkService));
}

void WebNetworkServicesManagerProxy::didReceiveNetworkServiceEvent(WebKit::WebNetworkService* networkService, const String& event)
{
    if (!context())
        return;

    context()->sendToAllProcesses(Messages::WebNetworkServicesManager::DidReceiveNetworkServiceEvent(*networkService, event));
}

void WebNetworkServicesManagerProxy::didReceiveDispatchExistingNetworkService(WebKit::WebNetworkService* networkService)
{
    if (!context())
        return;

    context()->sendToAllProcesses(Messages::WebNetworkServicesManager::DidReceiveDispatchExistingNetworkService(*networkService));
}

void WebNetworkServicesManagerProxy::didReceiveNotifyDiscoveryFinished()
{
    if (!context())
        return;

    context()->sendToAllProcesses(Messages::WebNetworkServicesManager::DidReceiveNotifyDiscoveryFinished());
}

// WebContextSupplement

void WebNetworkServicesManagerProxy::contextDestroyed()
{
    stopUpdating();
}

void WebNetworkServicesManagerProxy::processDidClose(WebProcessProxy*)
{
}

void WebNetworkServicesManagerProxy::refWebContextSupplement()
{
    API::Object::ref();
}

void WebNetworkServicesManagerProxy::derefWebContextSupplement()
{
    API::Object::deref();
}

void WebNetworkServicesManagerProxy::startUpdating()
{
    if (m_isUpdating)
        return;

    m_provider.startUpdating(this);
    m_isUpdating = true;
}

void WebNetworkServicesManagerProxy::stopUpdating()
{
    if (!m_isUpdating)
        return;

    m_provider.stopUpdating(this);
    m_isUpdating = false;
}

void WebNetworkServicesManagerProxy::sendDiscoveredServices()
{
    m_provider.sendDiscoveredServices(this);
}

void WebNetworkServicesManagerProxy::subscribeEvent(const String& id)
{
    if (!m_isUpdating)
        return;

    m_provider.subscribeEvent(this, id);
}

} // namespace WebKit

#endif // ENABLE(DISCOVERY)
