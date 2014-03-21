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
#include "NetworkServicesController.h"

#if ENABLE(DISCOVERY)

#include "Event.h"
#include "NavigatorNetworkService.h"
#include "NetworkServicesClient.h"
#include "NetworkServicesRequest.h"

namespace WebCore {

NetworkServicesController::NetworkServicesController(NetworkServicesClient* client)
    : m_client(client)
{
    ASSERT(m_client);
}

NetworkServicesController::~NetworkServicesController()
{
}

PassOwnPtr<NetworkServicesController> NetworkServicesController::create(NetworkServicesClient* client)
{
    return adoptPtr(new NetworkServicesController(client));
}

void NetworkServicesController::addObserver(NetworkServicesManager* observer)
{
    m_client->startUpdating();

    if (!m_observers.contains(observer))
        m_observers.add(observer);
}

void NetworkServicesController::removeObserver(NetworkServicesManager* observer)
{
    if (!m_observers.contains(observer))
        return;

    m_observers.remove(observer);

    if (m_observers.isEmpty())
        m_client->stopUpdating();
}

void NetworkServicesController::initializeRequest(PassRefPtr<NetworkServicesRequest> request)
{
    m_currentRequest = request;
    m_client->initializeRequest(m_currentRequest);
}

bool NetworkServicesController::discoveryFinished()
{
    bool oneCurrentRequest = false;
    Vector<NetworkServicesManager*> observersVector;
    copyToVector(m_observers, observersVector);
    for (size_t i = 0; i < observersVector.size(); ++i)
        if (observersVector[i]->discoveryFinished())
            oneCurrentRequest = true;
    return oneCurrentRequest;
}

void NetworkServicesController::requestUpdated(PassRefPtr<NetworkServicesRequest> request)
{
    if (m_client)
        m_client->requestUpdated(request);
}

void NetworkServicesController::requestCanceled(PassRefPtr<NetworkServicesRequest> request)
{
    if (m_client)
        m_client->requestCanceled(request);
}

void NetworkServicesController::subscribeEvent(const String& id)
{
    if (m_client)
        m_client->subscribeEvent(id);
}

void NetworkServicesController::networkServiceChanged(NetworkServiceDescription* networkServiceDesc)
{
    m_lastNetworkServiceDesc = networkServiceDesc;
    Vector<NetworkServicesManager*> observersVector;
    copyToVector(m_observers, observersVector);
    for (size_t i = 0; i < observersVector.size(); ++i)
        observersVector[i]->networkServiceChanged();
}

void NetworkServicesController::networkServiceInitializeRequest(NetworkServiceDescription* networkServiceDesc)
{
    m_lastNetworkServiceDesc = networkServiceDesc;
    m_currentRequest->networkServiceInitialized();
}

void NetworkServicesController::networkServiceNotifyEvent(NetworkServiceDescription* networkServiceDesc, const String& data)
{
    Vector<NetworkServicesManager*> observersVector;
    copyToVector(m_observers, observersVector);
    m_lastNetworkServiceDesc = networkServiceDesc;
    m_lastNotifyEvent = NotifyEvent::create(data);
    for (size_t i = 0; i < observersVector.size(); ++i)
        observersVector[i]->networkServiceNotifyEvent();
}

NetworkServiceDescription* NetworkServicesController::lastNetworkServiceDescription()
{
    return m_lastNetworkServiceDesc;
}

NotifyEvent* NetworkServicesController::lastNotifyEvent()
{
    if (m_lastNotifyEvent.get())
        return m_lastNotifyEvent.get();

    return 0;
}

const char* NetworkServicesController::supplementName()
{
    return "NetworkServicesController";
}

void provideNetworkServicesTo(Page* page, NetworkServicesClient* client)
{
    Supplement<Page>::provideTo(page, NetworkServicesController::supplementName(), NetworkServicesController::create(client));
}

}

#endif // DISCOVERY
