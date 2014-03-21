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

#ifndef NetworkServicesController_h
#define NetworkServicesController_h

#if ENABLE(DISCOVERY)

#include "NetworkServicesManager.h"
#include "NotifyEvent.h"
#include "Page.h"
#include <wtf/HashSet.h>
#include <wtf/Noncopyable.h>
#include <wtf/RefPtr.h>

namespace WebCore {

class NavigatorNetworkService;
class NetworkServicesClient;
class NetworkServicesRequest;

class NetworkServicesController : public Supplement<Page> {
    WTF_MAKE_NONCOPYABLE(NetworkServicesController);    
public:
    ~NetworkServicesController();

    static PassOwnPtr<NetworkServicesController> create(NetworkServicesClient*);

    void addObserver(NetworkServicesManager*);
    void removeObserver(NetworkServicesManager*);

    void initializeRequest(PassRefPtr<NetworkServicesRequest>);
    bool discoveryFinished();
    
    void requestUpdated(PassRefPtr<NetworkServicesRequest>);
    void requestCanceled(PassRefPtr<NetworkServicesRequest>);

    void subscribeEvent(const String& id);

    void networkServiceChanged(NetworkServiceDescription*);
    void networkServiceInitializeRequest(NetworkServiceDescription*);
    void networkServiceNotifyEvent(NetworkServiceDescription*, const String&);

    NetworkServiceDescription* lastNetworkServiceDescription();
    NotifyEvent* lastNotifyEvent();

    NetworkServicesClient* client() const { return m_client; }

    static const char* supplementName();
    static NetworkServicesController* from(Page* page) { return static_cast<NetworkServicesController*>(Supplement<Page>::from(page, supplementName())); }
    
private:
    explicit NetworkServicesController(NetworkServicesClient*);

    NetworkServicesClient* m_client;

    NetworkServiceDescription* m_lastNetworkServiceDesc;
    RefPtr<NotifyEvent> m_lastNotifyEvent;
    typedef HashSet<NetworkServicesManager*> ObserversSet;
    ObserversSet m_observers;
    RefPtr<NetworkServicesRequest> m_currentRequest;
};

}

#endif // DISCOVERY
#endif // NetworkServicesController_h
