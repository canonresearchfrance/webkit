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
#include "NetworkServicesProviderGlib.h"

#if ENABLE(DISCOVERY)

namespace WebCore {

NetworkServicesProviderGlib::NetworkServicesProviderGlib(NetworkServicesProviderClient* client)
    : m_client(client)
    , m_currentProvider(0)
    , m_gssdpProvider(this)
    , m_avahiProvider(this)
    , m_finishedMask(0)
    , m_allProviderFinishedOnce(false)
    , m_started(false)
{
}

void NetworkServicesProviderGlib::startUpdating() 
{
    m_gssdpProvider.startUpdating();
    m_avahiProvider.startUpdating();
}

void NetworkServicesProviderGlib::stopUpdating() 
{
    m_gssdpProvider.stopUpdating();
    m_avahiProvider.stopUpdating();

    m_started = false; 
}

void NetworkServicesProviderGlib::sendDiscoveredServices() 
{
    m_gssdpProvider.sendDiscoveredServices();
    m_avahiProvider.sendDiscoveredServices();

    if (m_started && m_allProviderFinishedOnce) {
        m_finishedMask = AllProviderFinished;
        notifyDiscoveryFinished();
    } else
        m_started = true;
}

void NetworkServicesProviderGlib::subscribeEvent(const String& id)
{
    if (id.startsWith("uuid:"))
        m_gssdpProvider.subscribeEventNotification(id);
}

void NetworkServicesProviderGlib::setCurrentProvider(WebCore::NetworkServicesProviderBase* provider)
{
    m_currentProvider = provider;
}

void NetworkServicesProviderGlib::notifyDiscoveryFinished()
{
    if (m_currentProvider == &m_gssdpProvider)
        m_finishedMask |= GssdpProviderFinished;
    else if (m_currentProvider == &m_avahiProvider)
        m_finishedMask |= AvahiProviderFinished;

    if (m_finishedMask == AllProviderFinished) {
        m_finishedMask = 0;       
        m_allProviderFinishedOnce = true;
        m_client->notifyDiscoveryFinished();
    }
}

void NetworkServicesProviderGlib::notifyNetworkServiceChanged(WebCore::NetworkServiceDescription* desc)
{
    m_client->notifyNetworkServiceChanged(desc);
}

void NetworkServicesProviderGlib::notifyNetworkServiceEvent(WebCore::NetworkServiceDescription* desc, const String& event)
{
    m_client->notifyNetworkServiceEvent(desc, event);
}

void NetworkServicesProviderGlib::dispatchExistingNetworkService(WebCore::NetworkServiceDescription* desc)
{
   m_client->dispatchExistingNetworkService(desc);
}

}

#endif // ENABLE(DISCOVERY)
