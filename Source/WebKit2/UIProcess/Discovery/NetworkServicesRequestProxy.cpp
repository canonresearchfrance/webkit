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
#include "NetworkServicesRequestProxy.h"

#include "NetworkServicesRequestManagerProxy.h"

namespace WebKit {

NetworkServicesRequestProxy::NetworkServicesRequestProxy(NetworkServicesRequestManagerProxy* manager, uint64_t requestID)
    : m_manager(manager)
    , m_requestID(requestID)
    , m_valid(false)
{
    m_webNetworkServices = WebNetworkServices::create();
}

WebNetworkServices* NetworkServicesRequestProxy::webNetworkServices() 
{ 
    if (!m_valid) {
        m_manager->getNetworkServices(m_requestID, m_webNetworkServices->data());
        m_valid = true;
    }
        
    return m_webNetworkServices.get(); 
}

unsigned long NetworkServicesRequestProxy::length() 
{ 
    return webNetworkServices()->length(); 
}

unsigned long NetworkServicesRequestProxy::availableServices()
{ 
    return webNetworkServices()->availableServices(); 
}

String NetworkServicesRequestProxy::origin()
{ 
    return webNetworkServices()->origin(); 
}

const WebNetworkService* NetworkServicesRequestProxy::getServiceByIndex(unsigned long index ) 
{
    return webNetworkServices()->getServiceByIndex(index); 
}

void NetworkServicesRequestProxy::setServiceAllowed(const String& serviceId, bool allowed)
{
    if (!m_manager)
        return;

    m_manager->didReceiveNetworkServiceAllowance(m_requestID, serviceId, allowed);
}

void NetworkServicesRequestProxy::allow()
{
    if (!m_manager)
        return;

    m_manager->didReceiveNetworkServicesPermissionDecision(m_requestID, true);
}

void NetworkServicesRequestProxy::deny()
{
    if (!m_manager)
        return;

    m_manager->didReceiveNetworkServicesPermissionDecision(m_requestID, false);
}

void NetworkServicesRequestProxy::invalidate()
{
    m_valid = false;
}

} // namespace WebKit
