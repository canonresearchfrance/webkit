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
#include "NetworkServicesRequest.h"

#if ENABLE(DISCOVERY)

#include "Document.h"
#include "Event.h"
#include "EventNames.h"
#include "NavigatorNetworkServiceError.h"
#include "NavigatorNetworkServiceErrorCallback.h"
#include "NavigatorNetworkServiceSuccessCallback.h"
#include "NetworkService.h"
#include "NetworkServices.h"
#include "ScriptExecutionContext.h"
#include "SecurityOrigin.h"
#include "SecurityPolicy.h"
#include "URL.h"


namespace WebCore {

NetworkServicesRequest::NetworkServicesRequest(NetworkServicesManager* client, ScriptExecutionContext* context, PassRefPtr<NavigatorNetworkServiceSuccessCallback> successCallback, PassRefPtr<NavigatorNetworkServiceErrorCallback> errorCallback, Vector<String>& types)
    : m_client(client)
    , m_scriptExecutionContext(context)
    , m_successCallback(successCallback)
    , m_errorCallback(errorCallback)
    , m_types(types)
    , m_discoveryFinished(false)
    , m_requestStatus(InProgress)
{
    m_networkServices = NetworkServices::create(0);
    m_dispatchNetworkServices = NetworkServices::create(context);
}

NetworkServicesRequest::~NetworkServicesRequest()
{
}

void NetworkServicesRequest::networkServiceInitialized()
{
    ASSERT(lastNetworkServiceDescription());

    NetworkServiceDescription* lastServiceDesc = lastNetworkServiceDescription();

    if (!containsType(lastServiceDesc->type()))
        return;

    if (lastServiceDesc->online())
        addService(lastServiceDesc);
}

void NetworkServicesRequest::networkServiceChanged()
{
    RefPtr<NetworkService> service;
    bool match;
    bool online;
    NetworkServiceDescription* lastServiceDesc = lastNetworkServiceDescription();

    if (!lastServiceDesc)
        return;

    match = containsType(lastServiceDesc->type());
    if (!match)
        return;
    
    online = lastServiceDesc->online();


    // Check if the lastServiceDesc has been already add in the services list
    service = m_networkServices->getServiceById(lastServiceDesc->id());

    // Update service
    if (service)
        service->update(String(lastServiceDesc->url()), String(lastServiceDesc->config()));
    
    switch (m_requestStatus) {
    case InProgress:
        // success or error callbacks not yet call. 
        if (!service) 
            addService(lastServiceDesc);
        else
            service->setOnline(online);

        if (m_discoveryFinished)
            m_client->requestUpdated(this);
        break;
    
    case Finished:
        {        
            RefPtr<NetworkService> dispatchService;

            // success or error callbacks has been call. 
            // The m_networkServices shall be updated, service online status and servicesAvailable counter.
            if (service) {
                // Update online status
                service->setOnline(online);
            } else {
                // Should be optimize ???
                // (replace by increment or decrement m_networkServices.servicesAvailable counter)
                addService(lastServiceDesc);
            }

            // Is it really needed ??? dispatchService is a ref to service !!!
            dispatchService = m_dispatchNetworkServices->getServiceById(lastServiceDesc->id());
            if (dispatchService)
                dispatchService->setOnline(online);

            m_dispatchNetworkServices->setServicesAvailable(m_networkServices->servicesAvailable());
        }
        break;
    }
}

void NetworkServicesRequest::networkServiceNotifyEvent()
{
    RefPtr<NetworkService> service;
    bool match;
    NetworkServiceDescription* lastServiceDesc;

    if (m_requestStatus == InProgress)
        return;

    lastServiceDesc = lastNetworkServiceDescription();
    if (!lastServiceDesc)
        return;

    match = containsType(lastServiceDesc->type());
    if (!match)
        return;

    // Check if the lastServiceDesc has been already add in the services list
    service = m_networkServices->getServiceById(lastServiceDesc->id());

    if (service)
        service->sendNotifyEvent(lastNotifyEvent());
}

NetworkServiceDescription* NetworkServicesRequest::lastNetworkServiceDescription()
{
    return m_client->lastNetworkServiceDescription();
}

NotifyEvent* NetworkServicesRequest::lastNotifyEvent()
{
    return m_client->lastNotifyEvent();
}

bool NetworkServicesRequest::containsType(const String& type) const
{
    return m_types.contains(type);
}

void NetworkServicesRequest::addService(NetworkServiceDescription* service)
{
    RefPtr<NetworkService> newService;
    
    if (m_requestStatus == InProgress)
        newService = NetworkService::create(m_scriptExecutionContext, service);
    else
        newService = NetworkService::create(0, service);

    m_networkServices->append(newService);
}

void NetworkServicesRequest::setAllowed()
{
    if (m_requestStatus != InProgress)
        return;

    // Copy network service ref from m_networkServices to m_dispatchNetworkServices
    // NetworkService online status can change after the notification.
    for (size_t i = 0; i < m_networkServices->length(); i++) {
        RefPtr<NetworkService> service = m_networkServices->item(i);
        if (service->online() && service->isAllowed())
            m_dispatchNetworkServices->append(service);
    }

    m_client->resetCurrentRequest();

    runSuccessCallback();
}

void NetworkServicesRequest::setDenied()
{
    if (m_requestStatus != InProgress)
        return;

    m_client->resetCurrentRequest();

    runErrorCallback(NavigatorNetworkServiceError::create(NavigatorNetworkServiceError::PERMISSION_DENIED_ERR).get());
}

void NetworkServicesRequest::runSuccessCallback()
{
    // If we are here and the request status is Finished, something has
    // gone horribly wrong.
    if (m_requestStatus != InProgress)
        CRASH();
    
    m_requestStatus = Finished;

    // TODO: Implement preliminary CORS check
    // registerWhiteList();

    m_successCallback->handleEvent(m_dispatchNetworkServices.get());
}

void NetworkServicesRequest::runErrorCallback(NavigatorNetworkServiceError* error)
{
    // If we are here and the request status is Finished, something has
    // gone horribly wrong.
    if (m_requestStatus != InProgress)
        CRASH();

    m_requestStatus = Finished;

    if (m_errorCallback)
        m_errorCallback->handleEvent(error);
}

void NetworkServicesRequest::resetWhiteList() const 
{
    NetworkServices* services = m_dispatchNetworkServices.get();
    
    for (size_t i = 0; i < services->length(); i++) {
        const URL destination(URL(), services->item(i)->url());
        SecurityPolicy::removeOriginAccessWhitelistEntry(*m_scriptExecutionContext->securityOrigin(),
            destination.protocol(), destination.host(), 1);
    }
}

void NetworkServicesRequest::registerWhiteList() const
{
    NetworkServices* services = m_dispatchNetworkServices.get();
    
    /* make all service URL white-listed */
    for (size_t i = 0; i < services->length(); i++) {
        const URL destination(URL(), services->item(i)->url());
        SecurityPolicy::addOriginAccessWhitelistEntry(*m_scriptExecutionContext->securityOrigin(),
            destination.protocol(), destination.host(), 1);
    }
}

}

#endif
