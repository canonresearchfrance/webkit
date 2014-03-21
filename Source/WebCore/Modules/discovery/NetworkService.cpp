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
#include "NetworkService.h"

#if ENABLE(DISCOVERY)

#include "Document.h"
#include "Event.h"
#include "NetworkServicesController.h"
#include "NotifyEvent.h"

namespace WebCore {

PassRefPtr<NetworkService> NetworkService::create(ScriptExecutionContext* context, NetworkServiceDescription* desc)
{
    RefPtr<NetworkService> networkService(adoptRef(new NetworkService(context, desc)));
    networkService->suspendIfNeeded();
    return networkService.release();
}

NetworkService::NetworkService(ScriptExecutionContext *context, const NetworkServiceDescription* desc)
    : ActiveDOMObject(context)
    , m_desc(*desc)
    , m_allowed(true)
{
}

void NetworkService::setOnline(bool online) 
{ 
    if (online == m_desc.online())
        return;
    
    m_desc.setOnline(online); 

    if (!scriptExecutionContext() || !m_allowed)
        return;

    /* Send NetworkService event */
    if (m_desc.online())
        dispatchEvent(Event::create(eventNames().availableEvent, false, false));
    else
        dispatchEvent(Event::create(eventNames().unavailableEvent, false, false));
}

void NetworkService::sendNotifyEvent(NotifyEvent* event)
{
    RefPtr<NotifyEvent> rpEvent = event;

    if (!scriptExecutionContext())
        return;

    dispatchEvent(rpEvent);
}

void NetworkService::update(const String& url, const String& config)
{
    m_desc.update(url, config);
}

Document* NetworkService::document() const
{
    ASSERT(!scriptExecutionContext() || scriptExecutionContext()->isDocument());
    return static_cast<Document*>(scriptExecutionContext());
}

Page* NetworkService::page() const
{
    return document() ? document()->page() : 0;
}

void NetworkService::setOnnotify(PassRefPtr<EventListener> listener) 
{
    NetworkServicesController* ctrl = NetworkServicesController::from(page());

    if (ctrl)
        ctrl->subscribeEvent(m_desc.id());

    setAttributeEventListener(eventNames().notifyEvent, listener);
}

void NetworkService::suspend(ReasonForSuspension)
{
}

void NetworkService::resume()
{
}

void NetworkService::stop()
{
}

} // namespace WebCore

#endif // ENABLE(DISCOVERY)
