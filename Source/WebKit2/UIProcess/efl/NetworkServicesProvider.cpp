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
#include "NetworkServicesProvider.h"

#if ENABLE(DISCOVERY)

#include "WKAPICast.h"
#include "WKNetworkServicesManager.h"
#include "WebNetworkService.h"

using namespace WebCore;
using namespace WebKit;

static inline NetworkServicesProvider* toNetworkServicesProvider(const void* clientInfo)
{
    return static_cast<NetworkServicesProvider*>(const_cast<void*>(clientInfo));
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

NetworkServicesProvider::~NetworkServicesProvider()
{
    m_provider.stopUpdating();

    WKNetworkServicesManagerSetProvider(m_manager.get(), 0);
}

PassRefPtr<NetworkServicesProvider> NetworkServicesProvider::create(WKContextRef context)
{
    ASSERT(context);
    return adoptRef(new NetworkServicesProvider(context));
}

NetworkServicesProvider::NetworkServicesProvider(WKContextRef context)
    : m_manager(WKContextGetNetworkServicesManager(context))
    , m_provider(this)
{
    ASSERT(m_manager);

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

    WKNetworkServicesManagerSetProvider(m_manager.get(), &wkNetworkServicesProvider.base);
}

void NetworkServicesProvider::startUpdating()
{
    m_provider.startUpdating();
}

void NetworkServicesProvider::stopUpdating()
{
    m_provider.stopUpdating();
}

void NetworkServicesProvider::sendDiscoveredServices()
{
    m_provider.sendDiscoveredServices();
}

void NetworkServicesProvider::subscribeEvent(const String& id)
{
    m_provider.subscribeEvent(id);
}

void NetworkServicesProvider::notifyDiscoveryFinished()
{
    WKNetworkServicesManagerProviderNotifyDiscoveryFinished(m_manager.get());
}

void NetworkServicesProvider::notifyNetworkServiceChanged(NetworkServiceDescription* desc)
{
    WebNetworkService* service = new WebNetworkService(desc->id(), 
        desc->name(), 
        desc->type(), 
        desc->url(), 
        desc->config(), 
        desc->online(),
        desc->corsEnable());

    WKNetworkServicesManagerProviderNotifyNetworkServiceChanged(m_manager.get(), toAPI(service));

    delete service;
}

void NetworkServicesProvider::notifyNetworkServiceEvent(NetworkServiceDescription* desc, const String& event)
{
    WebNetworkService* service = new WebNetworkService(desc->id(), 
        desc->name(), 
        desc->type(), 
        desc->url(), 
        desc->config(), 
        desc->online(),
        desc->corsEnable());

    WKNetworkServicesManagerProviderNotifyNetworkServiceEvent(m_manager.get(), toAPI(service), adoptWK(toCopiedAPI(event)).get());

    delete service;
}

void NetworkServicesProvider::dispatchExistingNetworkService(NetworkServiceDescription* desc)
{
    WebNetworkService* service = new WebNetworkService(desc->id(), 
        desc->name(), 
        desc->type(), 
        desc->url(), 
        desc->config(), 
        desc->online(),
        desc->corsEnable());

    WKNetworkServicesManagerProviderDispatchExistingNetworkService(m_manager.get(), toAPI(service));

    delete service;

}


#endif // ENABLE(DISCOVERY)
