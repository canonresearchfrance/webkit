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

#ifndef NetworkServices_h
#define NetworkServices_h

#if ENABLE(DISCOVERY)

#include "ActiveDOMObject.h"
#include "DOMStringList.h"
#include "EventTarget.h"
#include "NavigatorNetworkServiceError.h"
#include "NavigatorNetworkServiceErrorCallback.h"
#include "NavigatorNetworkServiceSuccessCallback.h"
#include "NetworkService.h"

#include <wtf/PassRefPtr.h>
#include <wtf/RefCounted.h>
#include <wtf/Vector.h>

namespace WebCore {

class Document;
class Frame;
class NavigatorNetworkServiceSuccessCallback;
class NavigatorNetworkServiceErrorCallback;
class Page;
class ScriptExecutionContext;

class NetworkServices : public RefCounted<NetworkServices>, public ActiveDOMObject, public EventTarget {
public:
    static PassRefPtr<NetworkServices> create(ScriptExecutionContext*);

    virtual ~NetworkServices();

    unsigned long length() const { return m_services.size(); }
    unsigned long servicesAvailable(); 

    PassRefPtr<NetworkService> item(unsigned long index) const;
    PassRefPtr<NetworkService> getServiceById(const String& id) const;

    void setServicesAvailable(unsigned long);

    Document* document() const;

    // event handler attributes
    DEFINE_ATTRIBUTE_EVENT_LISTENER(servicefound);
    DEFINE_ATTRIBUTE_EVENT_LISTENER(servicelost);

    // EventTarget impl
    using RefCounted<NetworkServices>::ref;
    using RefCounted<NetworkServices>::deref;

    virtual EventTargetInterface eventTargetInterface() const { return NetworkServicesEventTargetInterfaceType; }
    virtual ScriptExecutionContext* scriptExecutionContext() const { return ActiveDOMObject::scriptExecutionContext(); }

    // NetworkService handling
    void append(PassRefPtr<NetworkService>);
    void remove(PassRefPtr<NetworkService>);

    // ActiveDOMObject implementation.
    virtual bool canSuspend() const { return true; }
    virtual void suspend(ReasonForSuspension);
    virtual void resume();
    virtual void stop();

protected:
    NetworkServices(ScriptExecutionContext*);

private:

    // EventTarget impl
    virtual void refEventTarget() { ref(); }
    virtual void derefEventTarget() { deref(); }
    virtual EventTargetData* eventTargetData() { return &m_eventTargetData; }
    virtual EventTargetData& ensureEventTargetData() { return m_eventTargetData; }
    EventTargetData m_eventTargetData;

    NetworkServices* lastNetworkServices();

    unsigned long m_servicesAvailable;
    Vector<RefPtr<NetworkService> > m_services;
};

} // namespace WebCore

#endif // ENABLE(DISCOVERY)

#endif // NetworkServices_h
