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
#include "NetworkServicesClientEfl.h"

#if ENABLE(DISCOVERY)

#include "NetworkServicesController.h"
#include "Page.h"
#include "ewk_network_services_private.h"

NetworkServicesClientEfl::NetworkServicesClientEfl(Evas_Object* view, WebCore::Page* page)
    : m_view(view)
    , m_page(page)
    , m_provider(this)
{
}

NetworkServicesClientEfl::~NetworkServicesClientEfl()
{
    m_provider.stopUpdating();
}

void NetworkServicesClientEfl::networkServicesDestroyed()
{
    delete this;
}

void NetworkServicesClientEfl::startUpdating()
{
    m_provider.startUpdating();
}

void NetworkServicesClientEfl::stopUpdating()
{
    m_provider.stopUpdating();
}

void NetworkServicesClientEfl::initializeRequest(PassRefPtr<WebCore::NetworkServicesRequest> request)
{
    notifyDiscoveryStarted(request);

    m_provider.sendDiscoveredServices();
}

void NetworkServicesClientEfl::requestUpdated(PassRefPtr<WebCore::NetworkServicesRequest> request)
{
    Ewk_NetworkServices* ewkServices = ewk_network_services_get(request);

    evas_object_smart_callback_call(m_view, "networkservices,request,updated", ewkServices);
}

void NetworkServicesClientEfl::requestCanceled(PassRefPtr<WebCore::NetworkServicesRequest> request)
{
    Ewk_NetworkServices* ewkServices = ewk_network_services_get(request);

    evas_object_smart_callback_call(m_view, "networkservices,request,canceled", ewkServices);
}

void NetworkServicesClientEfl::subscribeEvent(const String& id)
{
    m_provider.subscribeEvent(id);
}

void NetworkServicesClientEfl::notifyDiscoveryStarted(PassRefPtr<WebCore::NetworkServicesRequest> request)
{
    Ewk_NetworkServices* ewkServices = ewk_network_services_new(request);

    evas_object_smart_callback_call(m_view, "networkservices,request,started", ewkServices);
}

void NetworkServicesClientEfl::notifyDiscoveryFinished()
{
    WebCore::NetworkServicesController::from(m_page)->discoveryFinished();

    evas_object_smart_callback_call(m_view, "networkservices,request,finished", 0);
}

void NetworkServicesClientEfl::notifyNetworkServiceChanged(WebCore::NetworkServiceDescription* networkServiceDesc)
{
    WebCore::NetworkServicesController::from(m_page)->networkServiceChanged(networkServiceDesc);
}

void NetworkServicesClientEfl::notifyNetworkServiceEvent(WebCore::NetworkServiceDescription* networkServiceDesc, const String& data)
{
    WebCore::NetworkServicesController::from(m_page)->networkServiceNotifyEvent(networkServiceDesc, data);
}

void NetworkServicesClientEfl::dispatchExistingNetworkService(WebCore::NetworkServiceDescription* networkServiceDesc)
{
    WebCore::NetworkServicesController::from(m_page)->networkServiceInitializeRequest(networkServiceDesc);
}

#endif // ENABLE(DISCOVERY)
