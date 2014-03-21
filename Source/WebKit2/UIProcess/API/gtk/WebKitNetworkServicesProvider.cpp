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
#include "WebKitNetworkServicesProvider.h"

#if ENABLE(DISCOVERY)

#include "WebNetworkService.h"
#include "WebNetworkServicesManagerProxy.h"

using namespace WebCore;
using namespace WebKit;

static inline WebKitNetworkServicesProvider* toNetworkServicesProvider(const void* clientInfo)
{
    return static_cast<WebKitNetworkServicesProvider*>(const_cast<void*>(clientInfo));
}

static void startUpdatingCallback(WKNetworkServicesManagerRef, const void* clientInfo)
{
    toNetworkServicesProvider(clientInfo)->startUpdating();
}

static void stopUpdatingCallback(WKNetworkServicesManagerRef, const void* clientInfo)
{
    toNetworkServicesProvider(clientInfo)->stopUpdating();
}

static void sendDiscoveredServicesCallback(WKNetworkServicesManagerRef, const void* clientInfo)
{
    toNetworkServicesProvider(clientInfo)->sendDiscoveredServices();
}

static void subscribeEventCallback(WKNetworkServicesManagerRef, WKStringRef id, const void* clientInfo)
{
    toNetworkServicesProvider(clientInfo)->subscribeEvent(toWTFString(id));
}

WebKitNetworkServicesProvider::~WebKitNetworkServicesProvider()
{
    m_provider.stopUpdating();

    WKNetworkServicesManagerSetProvider(toAPI(m_manager.get()), 0);
}

PassRefPtr<WebKitNetworkServicesProvider> WebKitNetworkServicesProvider::create(WebNetworkServicesManagerProxy* manager)
{
    return adoptRef(new WebKitNetworkServicesProvider(manager));
}

WebKitNetworkServicesProvider::WebKitNetworkServicesProvider(WebNetworkServicesManagerProxy* manager)
    : m_manager(manager)
    , m_provider(this)
{
    ASSERT(manager);

    WKNetworkServicesProviderV0 wkNetworkServicesProvider = {
        {
            0, // version
            this, // clientInfo
        },
        startUpdatingCallback,
        stopUpdatingCallback,
        sendDiscoveredServicesCallback,
        subscribeEventCallback
    };

    WKNetworkServicesManagerSetProvider(toAPI(manager), &wkNetworkServicesProvider.base);
}

void WebKitNetworkServicesProvider::startUpdating()
{
    m_provider.startUpdating();
}

void WebKitNetworkServicesProvider::stopUpdating()
{
    m_provider.stopUpdating();
}

void WebKitNetworkServicesProvider::sendDiscoveredServices()
{
    m_provider.sendDiscoveredServices();
}

void WebKitNetworkServicesProvider::subscribeEvent(const String& id)
{
    m_provider.subscribeEvent(id);
}

void WebKitNetworkServicesProvider::notifyDiscoveryFinished()
{
    m_manager->didReceiveNotifyDiscoveryFinished();
}

void WebKitNetworkServicesProvider::notifyNetworkServiceChanged(NetworkServiceDescription* desc)
{
    WebNetworkService* service = new WebNetworkService(desc->id(), 
        desc->name(), 
        desc->type(), 
        desc->url(), 
        desc->config(), 
        desc->online());

    m_manager->didReceiveNetworkServiceChanged(service);

    delete service;
}

void WebKitNetworkServicesProvider::notifyNetworkServiceEvent(NetworkServiceDescription* desc, const String& event)
{
    WebNetworkService* service = new WebNetworkService(desc->id(), 
        desc->name(), 
        desc->type(), 
        desc->url(), 
        desc->config(), 
        desc->online());

    m_manager->didReceiveNetworkServiceEvent(service, event);

    delete service;
}

void WebKitNetworkServicesProvider::dispatchExistingNetworkService(NetworkServiceDescription* desc)
{
    WebNetworkService* service = new WebNetworkService(desc->id(), 
        desc->name(), 
        desc->type(), 
        desc->url(), 
        desc->config(), 
        desc->online());

    m_manager->didReceiveDispatchExistingNetworkService(service);

    delete service;

}

#endif // ENABLE(DISCOVERY)
