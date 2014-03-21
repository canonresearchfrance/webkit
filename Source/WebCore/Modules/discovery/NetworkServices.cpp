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
#include "NetworkServices.h"

#if ENABLE(DISCOVERY)

#include "Document.h"
#include "Event.h"
#include "Frame.h"
#include "NetworkServicesController.h"
#include "Page.h"

namespace WebCore {

PassRefPtr<NetworkServices> NetworkServices::create(ScriptExecutionContext* context)
{
    RefPtr<NetworkServices> networkServices(adoptRef(new NetworkServices(context)));
    networkServices->suspendIfNeeded();
    return networkServices.release();
}

NetworkServices::NetworkServices(ScriptExecutionContext* context)
    : ActiveDOMObject(context)
    , m_servicesAvailable(0) 
{
}

NetworkServices::~NetworkServices()
{
}

void NetworkServices::append(PassRefPtr<NetworkService> service)
{
    if (m_services.contains(service))
        return;

    m_services.append(service);
    m_servicesAvailable++;
}

unsigned long NetworkServices::servicesAvailable() 
{
    unsigned long servicesAvailable = 0;

    int cptr, end = m_services.size();
    for (cptr = 0; cptr < end; cptr++) {
        if (m_services[cptr]->online())
            servicesAvailable++;
    }

    m_servicesAvailable = servicesAvailable; 

    return m_servicesAvailable;
}

void NetworkServices::remove(PassRefPtr<NetworkService> service)
{
    size_t index = m_services.find(service);
    m_services.remove(index);
}

PassRefPtr<NetworkService> NetworkServices::item(unsigned long index) const
{
    if (index >= m_services.size())
        return 0;
    return m_services[index];
}

PassRefPtr<NetworkService> NetworkServices::getServiceById(const String& id) const
{
    int cptr, end = m_services.size();
    for (cptr = 0; cptr < end; cptr++) {
        if (id == m_services[cptr].get()->id())
            return m_services[cptr];
    }
    return 0;
}

void NetworkServices::setServicesAvailable(unsigned long servicesAvailable)
{
    unsigned long oldServicesAvailable = m_servicesAvailable; 

    m_servicesAvailable = servicesAvailable;

    if (oldServicesAvailable == servicesAvailable)
        return;
    
    if (oldServicesAvailable < servicesAvailable)
        dispatchEvent(Event::create(eventNames().servicefoundEvent, false, false));
    else
        dispatchEvent(Event::create(eventNames().servicelostEvent, false, false));
}

Document* NetworkServices::document() const
{
    ASSERT(!scriptExecutionContext() || scriptExecutionContext()->isDocument());
    return static_cast<Document*>(scriptExecutionContext());
}

void NetworkServices::suspend(ReasonForSuspension)
{
}

void NetworkServices::resume()
{
}

void NetworkServices::stop()
{
}

} // namespace

#endif // ENABLE(DISCOVERY)
