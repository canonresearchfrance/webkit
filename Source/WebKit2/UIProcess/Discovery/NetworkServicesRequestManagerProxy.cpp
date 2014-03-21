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
#include "NetworkServicesRequestManagerProxy.h"

#include "WebPageMessages.h"
#include "WebPageProxy.h"
#include "WebProcessProxy.h"

namespace WebKit {

NetworkServicesRequestManagerProxy::NetworkServicesRequestManagerProxy(WebPageProxy& page)
    : m_page(page)
{
}

PassRefPtr<NetworkServicesRequestProxy> NetworkServicesRequestManagerProxy::getRequest(uint64_t requestID)
{
    auto it = m_pendingRequests.find(requestID); 
    if (it != m_pendingRequests.end())
        return it->value;

    return 0;
}

void NetworkServicesRequestManagerProxy::invalidateRequests()
{
    for (auto request : m_pendingRequests.values())
        request->invalidate();

    m_pendingRequests.clear();
}

PassRefPtr<NetworkServicesRequestProxy> NetworkServicesRequestManagerProxy::createRequest(uint64_t requestID)
{
    RefPtr<NetworkServicesRequestProxy> request = NetworkServicesRequestProxy::create(this, requestID);
    m_pendingRequests.add(requestID, request.get());
    return request.release();
}

void NetworkServicesRequestManagerProxy::didReceiveNetworkServiceAllowance(uint64_t requestID, const String& serviceId, bool allow)
{
    if (!m_page.isValid())
        return;

    auto it = m_pendingRequests.find(requestID);
    if (it == m_pendingRequests.end())
        return;

#if ENABLE(DISCOVERY)
    m_page.process().send(Messages::WebPage::DidReceiveNetworkServiceAllowance(requestID, serviceId, allow), m_page.pageID());
#else
    UNUSED_PARAM(serviceId);
    UNUSED_PARAM(allow);
#endif
}

void NetworkServicesRequestManagerProxy::didReceiveNetworkServicesPermissionDecision(uint64_t requestID, bool allow)
{
    if (!m_page.isValid())
        return;

    auto it = m_pendingRequests.find(requestID);
    if (it == m_pendingRequests.end())
        return;

#if ENABLE(DISCOVERY)
    m_page.process().send(Messages::WebPage::DidReceiveNetworkServicesPermissionDecision(requestID, allow), m_page.pageID());
#else
    UNUSED_PARAM(allow);
#endif

    m_pendingRequests.remove(it);
}

void NetworkServicesRequestManagerProxy::getNetworkServices(uint64_t requestID, WebNetworkServices::Data& data)
{
    if (!m_page.isValid())
        return;

    auto it = m_pendingRequests.find(requestID);
    if (it == m_pendingRequests.end())
        return;

#if ENABLE(DISCOVERY)
    m_page.process(). sendSync(Messages::WebPage::GetNetworkServices(requestID), Messages::WebPage::GetNetworkServices::Reply(data), m_page.pageID());
#else
    UNUSED_PARAM(data);
#endif
}

}
