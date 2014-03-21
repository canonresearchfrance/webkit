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
#include "NetworkServicesProviderBase.h"

#if ENABLE(DISCOVERY)


#include "NetworkServicesProviderClient.h"
#include <wtf/text/CString.h>
#include <wtf/text/WTFString.h>

namespace WebCore {

NetworkServicesProviderBase::NetworkServicesProviderBase(NetworkServicesProviderClient *client)
    : m_client(client)
    , m_currentDesc(0)
{
    ASSERT(m_client);    
}

void NetworkServicesProviderBase::sendDiscoveredServices()
{
    m_client->setCurrentProvider(this);

    for (size_t i = 0; i < m_descriptions.size(); i++) {
        m_currentDesc = m_descriptions[i];
        m_client->dispatchExistingNetworkService(m_currentDesc);
    }
}

void NetworkServicesProviderBase::setServiceEventSubURL(const String& id, const String& eventSubURL)
{
    getServiceDescriptionById(id)->setEventSubURL(eventSubURL);
}

NetworkServiceDescription* NetworkServicesProviderBase::getServiceDescriptionById(const String& id) const
{
    for (size_t i = 0; i < m_descriptions.size(); i++) {
        if (id == m_descriptions[i]->id())
            return m_descriptions[i];
    }
    return 0;
}

void NetworkServicesProviderBase::addServiceDescription(NetworkServiceDescription* desc)
{
    if (m_descriptions.contains(desc))
        return;

    m_descriptions.append(desc);
    m_currentDesc = desc;
    
    notifyNetworkServiceChanged(desc);
}

void NetworkServicesProviderBase::addServiceDescription(const char* id, const char* name, const char* type, const char* url, const char* config, const char* eventSubURL)
{
    NetworkServiceDescription* desc = new NetworkServiceDescription(id, name, type, url, config, eventSubURL);

    addServiceDescription(desc);
}

bool NetworkServicesProviderBase::contains(NetworkServiceDescription* desc)
{
    return m_descriptions.contains(desc);
}

void NetworkServicesProviderBase::removeServiceDescription(const char* id)
{
    m_currentDesc = getServiceDescriptionById(id);

    if (m_currentDesc) {
        m_currentDesc->setOnline(false);
    
        notifyNetworkServiceChanged(m_currentDesc);
    }
}

void NetworkServicesProviderBase::notifyDiscoveryFinished()
{
    m_client->setCurrentProvider(this);
    m_client->notifyDiscoveryFinished();
}

void NetworkServicesProviderBase::notifyNetworkServiceChanged(NetworkServiceDescription* desc)
{
    ASSERT(desc);

    m_client->setCurrentProvider(this);

    m_currentDesc = desc;
    m_client->notifyNetworkServiceChanged(m_currentDesc);
}

} // namespace WebCore

#endif // ENABLE(DISCOVERY)
