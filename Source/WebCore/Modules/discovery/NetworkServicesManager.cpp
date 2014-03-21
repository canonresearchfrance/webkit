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
#include "NetworkServicesManager.h"

#if ENABLE(DISCOVERY)

#include "Document.h"
#include "Event.h"
#include "EventNames.h"
#include "NetworkServicesClient.h"
#include "NetworkServicesController.h"
#include "ScriptExecutionContext.h"
#include "SecurityOrigin.h"
#include <wtf/text/CString.h>

namespace WebCore {

NetworkServicesManager::NetworkServicesManager(ScriptExecutionContext *c, PassRefPtr<NavigatorNetworkServiceSuccessCallback> successCallback, PassRefPtr<NavigatorNetworkServiceErrorCallback> errorCallback, Vector<String>& types)
    : m_scriptExecutionContext(c)
    , m_networkServicesController(0)
{
    ASSERT(m_scriptExecutionContext);
    ASSERT(this->page());

    startRequest(c, successCallback, errorCallback, types);

    m_networkServicesController = NetworkServicesController::from(this->page());
    m_networkServicesController->addObserver(this);
}

NetworkServicesManager::~NetworkServicesManager() 
{
    m_networkServicesController->removeObserver(this);
}

Document* NetworkServicesManager::document() const
{
    ASSERT(!scriptExecutionContext() || scriptExecutionContext()->isDocument());
    return static_cast<Document*>(scriptExecutionContext());
}

Page* NetworkServicesManager::page() const
{
    return document() ? document()->page() : 0;
}

void NetworkServicesManager::startRequest(ScriptExecutionContext *c, PassRefPtr<NavigatorNetworkServiceSuccessCallback> successCallback, PassRefPtr<NavigatorNetworkServiceErrorCallback> errorCallback, Vector<String>& types)
{
    ASSERT(successCallback);

    NetworkServicesController* ctrl;
    RefPtr<NetworkServicesRequest> request;
    Page* page = this->page();
    if (!page)
        return;

    ctrl = NetworkServicesController::from(page);

    request = NetworkServicesRequest::create(this, c, successCallback, errorCallback, types);

    if (m_currentRequest) {
        ctrl->requestCanceled(m_currentRequest);
        m_requests.remove(m_requests.find(m_currentRequest));
    }

    request->ref();
    m_requests.insert(0, request);
    ctrl->initializeRequest(request);
    m_currentRequest = request;
    request->deref();
}

NetworkServiceDescription* NetworkServicesManager::lastNetworkServiceDescription()
{
    Page* page = this->page();
    if (!page)
        return 0;

    return NetworkServicesController::from(page)->lastNetworkServiceDescription();
}

NotifyEvent* NetworkServicesManager::lastNotifyEvent()
{
    Page* page = this->page();
    if (!page)
        return 0;

    return NetworkServicesController::from(page)->lastNotifyEvent();
}

void NetworkServicesManager::requestUpdated(NetworkServicesRequest* request)
{
    Page* page = this->page();
    if (!page)
        return;

    /* This must never happen ! Only on request at a given time */
    if (request != m_currentRequest.get()) {
        ASSERT(0);
        return;
    }

    NetworkServicesController::from(page)->requestUpdated(m_currentRequest);
}

bool NetworkServicesManager::discoveryFinished()
{
    if (m_currentRequest) { 
        Page* page = this->page();
        if (!page)
            return false;

        m_currentRequest->discoveryFinished();
        return true;
    }        
    return false;
}

void NetworkServicesManager::update(PassRefPtr<NavigatorNetworkServiceSuccessCallback> successCallback, PassRefPtr<NavigatorNetworkServiceErrorCallback> errorCallback, Vector<String>& types)
{
    // If the current script reinvokes the getNetworkServices() method at any point in its execution: 
    // - remove all previously whitelisted urls from the entry script origin's URL whitelist.
    for (size_t i = 0; i < m_requests.size(); ++i)
        m_requests[i]->resetWhiteList();

    // Start a new request.
    startRequest(m_scriptExecutionContext, successCallback, errorCallback, types);
}

String NetworkServicesManager::getOrigin() const
{
    return m_scriptExecutionContext->securityOrigin()->toRawString(); 
}

void NetworkServicesManager::networkServiceChanged()
{
    for (size_t i = 0; i < m_requests.size(); ++i)
        m_requests[i]->networkServiceChanged();
}

void NetworkServicesManager::networkServiceNotifyEvent()
{
    for (size_t i = 0; i < m_requests.size(); ++i)
        m_requests[i]->networkServiceNotifyEvent();
}

}
#endif
