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
#include "NetworkServicesClientGtk.h"

#if ENABLE(DISCOVERY)

#include "NetworkServicesController.h"
#include "Page.h"
#include "webkitnetworkservices.h"
#include "webkitnetworkservicesprivate.h"

namespace WebKit {

NetworkServicesClient::NetworkServicesClient(WebKitWebView* view, WebCore::Page* page)
    : m_webView(view)
    , m_page(page)
    , m_provider(this)
{
}

NetworkServicesClient::~NetworkServicesClient()
{
    m_provider.stopUpdating();
}

void NetworkServicesClient::networkServicesDestroyed()
{
    delete this;
}

void NetworkServicesClient::startUpdating()
{
    m_provider.startUpdating();
}

void NetworkServicesClient::stopUpdating()
{
    m_provider.stopUpdating();
}

void NetworkServicesClient::initializeRequest(PassRefPtr<WebCore::NetworkServicesRequest> request)
{
    notifyDiscoveryStarted(request);

    m_provider.sendDiscoveredServices();
}

void NetworkServicesClient::requestUpdated(PassRefPtr<WebCore::NetworkServicesRequest> request)
{
    GRefPtr<WebKitNetworkServices> networkServices(adoptGRef(webkit_network_services_get(request)));

    g_signal_emit_by_name(m_webView, "networkservices-request-updated", networkServices.get());
}

void NetworkServicesClient::requestCanceled(PassRefPtr<WebCore::NetworkServicesRequest> request)
{
    GRefPtr<WebKitNetworkServices> networkServices(adoptGRef(webkit_network_services_get(request)));
    
    g_signal_emit_by_name(m_webView, "networkservices-request-canceled", networkServices.get());
}

void NetworkServicesClient::subscribeEvent(const String& id)
{
    m_provider.subscribeEvent(id);
}

void NetworkServicesClient::notifyDiscoveryStarted(PassRefPtr<WebCore::NetworkServicesRequest> request)
{
    GRefPtr<WebKitNetworkServices> networkServices(adoptGRef(webkit_network_services_new(request)));

    g_signal_emit_by_name(m_webView, "networkservices-request-started", networkServices.get());
}

void NetworkServicesClient::notifyDiscoveryFinished()
{
    WebCore::NetworkServicesController::from(m_page)->discoveryFinished();

    g_signal_emit_by_name(m_webView, "networkservices-request-finished");
}

void NetworkServicesClient::notifyNetworkServiceChanged(WebCore::NetworkServiceDescription* networkServiceDesc)
{
    WebCore::NetworkServicesController::from(m_page)->networkServiceChanged(networkServiceDesc);
}

void NetworkServicesClient::notifyNetworkServiceEvent(WebCore::NetworkServiceDescription* networkServiceDesc, const String& data)
{
    WebCore::NetworkServicesController::from(m_page)->networkServiceNotifyEvent(networkServiceDesc, data);
}

void NetworkServicesClient::dispatchExistingNetworkService(WebCore::NetworkServiceDescription* networkServiceDesc)
{
    WebCore::NetworkServicesController::from(m_page)->networkServiceInitializeRequest(networkServiceDesc);
}

}

#endif // ENABLE(DISCOVERY)
