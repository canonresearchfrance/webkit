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
#include "WebNetworkServicesClient.h"

#if ENABLE(DISCOVERY)

#include "NetworkServicesController.h"
#include "WebNetworkServicesManager.h"
#include "WebPage.h"
#include "WebProcess.h"

namespace WebKit {

WebNetworkServicesClient::WebNetworkServicesClient(WebPage* page)
    : m_page(page)
{
}

WebNetworkServicesClient::~WebNetworkServicesClient()
{
    stopUpdating();
}

void WebNetworkServicesClient::networkServicesDestroyed()
{
    delete this;
}

void WebNetworkServicesClient::startUpdating()
{
    WebProcess::shared().supplement<WebNetworkServicesManager>()->registerWebPage(m_page);
}

void WebNetworkServicesClient::stopUpdating()
{
    WebProcess::shared().supplement<WebNetworkServicesManager>()->unregisterWebPage(m_page);
}

void WebNetworkServicesClient::initializeRequest(PassRefPtr<WebCore::NetworkServicesRequest> request)
{
    notifyDiscoveryStarted(request);

    WebProcess::shared().supplement<WebNetworkServicesManager>()->sendDiscoveredServices();
}

void WebNetworkServicesClient::requestUpdated(PassRefPtr<WebCore::NetworkServicesRequest> request)
{
    m_page->networkServicesRequestManager().discoveryRequestUpdated(request.get());
}

void WebNetworkServicesClient::requestCanceled(PassRefPtr<WebCore::NetworkServicesRequest> request)
{
    m_page->networkServicesRequestManager().discoveryRequestCanceled(request.get());
}

void WebNetworkServicesClient::subscribeEvent(const String& id)
{
    if (id.startsWith("uuid:"))
        WebProcess::shared().supplement<WebNetworkServicesManager>()->subscribeEvent(id);
}

void WebNetworkServicesClient::notifyDiscoveryStarted(PassRefPtr<WebCore::NetworkServicesRequest> request)
{
    m_page->networkServicesRequestManager().discoveryRequestStarted(request.get());
}

void WebNetworkServicesClient::notifyDiscoveryFinished()
{
    m_page->networkServicesRequestManager().discoveryRequestFinished();
}

void WebNetworkServicesClient::notifyNetworkServiceChanged(WebCore::NetworkServiceDescription* networkServiceDesc)
{
    WebCore::NetworkServicesController::from(m_page->corePage())->networkServiceChanged(networkServiceDesc);
}

void WebNetworkServicesClient::notifyNetworkServiceEvent(WebCore::NetworkServiceDescription* networkServiceDesc, const String& data)
{
    WebCore::NetworkServicesController::from(m_page->corePage())->networkServiceNotifyEvent(networkServiceDesc, data);
}

void WebNetworkServicesClient::dispatchExistingNetworkService(WebCore::NetworkServiceDescription* networkServiceDesc)
{
    WebCore::NetworkServicesController::from(m_page->corePage())->networkServiceInitializeRequest(networkServiceDesc);
}

} // namespace WebKit

#endif // ENABLE(DISCOVERY)
