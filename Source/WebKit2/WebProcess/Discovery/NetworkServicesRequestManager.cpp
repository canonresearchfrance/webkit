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
#include "NetworkServicesRequestManager.h"

#if ENABLE(DISCOVERY)

#include "WebPage.h"
#include "WebPageProxyMessages.h"
#include <WebCore/NetworkServicesRequest.h>

using namespace WebCore;

namespace WebKit {

static uint64_t generateRequestID()
{
    static uint64_t uniqueRequestID = 1;
    return uniqueRequestID++;
}

NetworkServicesRequestManager::NetworkServicesRequestManager(WebPage* page)
    : m_page(page)
{
}

void NetworkServicesRequestManager::discoveryRequestStarted(NetworkServicesRequest* request)
{
#if ENABLE(DISCOVERY)
    uint64_t requestID = generateRequestID();

    m_requestToIDMap.set(request, requestID);
    m_idToRequestMap.set(requestID, request);

    m_page->sendSync(Messages::WebPageProxy::RequestNetworkServicesStarted(requestID), Messages::WebPageProxy::RequestNetworkServicesStarted::Reply());
#else
    UNUSED_PARAM(request);
#endif // ENABLE(DISCOVERY)
}

void NetworkServicesRequestManager::discoveryRequestFinished()
{
#if ENABLE(DISCOVERY)
    m_page->send(Messages::WebPageProxy::RequestNetworkServicesFinished());
#endif // ENABLE(DISCOVERY)
}

void NetworkServicesRequestManager::discoveryRequestUpdated(NetworkServicesRequest* request) const
{
#if ENABLE(DISCOVERY)
    uint64_t requestID = m_requestToIDMap.get(request);
    if (!requestID)
        return;

    m_page->sendSync(Messages::WebPageProxy::RequestNetworkServicesUpdated(requestID), Messages::WebPageProxy::RequestNetworkServicesUpdated::Reply());
#else
    UNUSED_PARAM(request);
#endif // ENABLE(DISCOVERY)
}

void NetworkServicesRequestManager::discoveryRequestCanceled(NetworkServicesRequest* request)
{
#if ENABLE(DISCOVERY)
    uint64_t requestID = m_requestToIDMap.get(request);
    if (!requestID)
        return;

    m_page->sendSync(Messages::WebPageProxy::RequestNetworkServicesCanceled(requestID), Messages::WebPageProxy::RequestNetworkServicesCanceled::Reply());
#else
    UNUSED_PARAM(request);
#endif // ENABLE(DISCOVERY)
}

void NetworkServicesRequestManager::getNetworkServices(uint64_t requestID, WebNetworkServices::Data& webNetworkServicesData)
{
    NetworkServicesRequest* request = m_idToRequestMap.get(requestID);

    if (!request)
        return;
    
    RefPtr<NetworkServices> networkServices = request->networkServices();
    String origin = request->client()->getOrigin();

    Vector<WebNetworkService> webServices;

    for (size_t i = 0; i < networkServices->length(); i++) {
        RefPtr<NetworkService> service = networkServices->item(i);
        webServices.append(WebNetworkService(service->id(), service->name(), service->type(), service->url(), service->config(), service->online(), service->isCorsEnable()));
    }

    RefPtr<WebNetworkServices> webNetworkServices = WebNetworkServices::create(networkServices->length(), networkServices->servicesAvailable(), origin, webServices);

    webNetworkServicesData = webNetworkServices->data();

    webNetworkServices.release();
}

void NetworkServicesRequestManager::didReceiveNetworkServiceAllowance(uint64_t requestID, const String& serviceID, bool allowed)
{
    NetworkServicesRequest* request = m_idToRequestMap.get(requestID);

    if (!request)
        return;

    RefPtr<NetworkService> service = request->networkServices()->getServiceById(serviceID);

    if (!service)
        return;

    service->setAllowed(allowed);
}

void NetworkServicesRequestManager::didReceiveNetworkServicesPermissionDecision(uint64_t requestID, bool allowed)
{
    NetworkServicesRequest* request = m_idToRequestMap.get(requestID);

    if (!request)
        return;

    if (allowed)
        request->setAllowed();
    else
        request->setDenied();
}

} // namespace WebKit

#endif
