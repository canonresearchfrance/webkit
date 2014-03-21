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

#ifndef NetworkServicesManager_h
#define NetworkServicesManager_h

#if ENABLE(DISCOVERY)

#include "NavigatorNetworkServiceErrorCallback.h"
#include "NavigatorNetworkServiceSuccessCallback.h"
#include "NetworkServices.h"
#include "NetworkServicesRequest.h"
#include "NotifyEvent.h"
#include <wtf/PassRefPtr.h>
#include <wtf/RefPtr.h>
#include <wtf/Vector.h>

namespace WebCore {

class Document;
class NavigatorNetworkServiceSuccessCallback;
class NavigatorNetworkServiceErrorCallback;
class NetworkServicesController;
class NetworkServicesRequest;
class Page;
class ScriptExecutionContext;

class NetworkServicesManager : public RefCounted<NetworkServicesManager> {
public:
    static PassRefPtr<NetworkServicesManager> create(ScriptExecutionContext *c, PassRefPtr<NavigatorNetworkServiceSuccessCallback> successCallback, PassRefPtr<NavigatorNetworkServiceErrorCallback> errorCallback, Vector<String>& types) 
    {
        return adoptRef(new NetworkServicesManager(c, successCallback, errorCallback, types));
    }

    ~NetworkServicesManager();

    void update(PassRefPtr<NavigatorNetworkServiceSuccessCallback>, PassRefPtr<NavigatorNetworkServiceErrorCallback>, Vector<String>&);

    ScriptExecutionContext* scriptExecutionContext() const { return m_scriptExecutionContext; }
    NetworkServicesController* controller() const { return m_networkServicesController; }

    NetworkServiceDescription * lastNetworkServiceDescription();
    NotifyEvent* lastNotifyEvent();

    String getOrigin() const;

    void networkServiceChanged();
    void networkServiceNotifyEvent();

    void requestUpdated(NetworkServicesRequest*);

    bool discoveryFinished();
    void resetCurrentRequest() { m_currentRequest = 0; }

private:
    NetworkServicesManager(ScriptExecutionContext*, PassRefPtr<NavigatorNetworkServiceSuccessCallback>, PassRefPtr<NavigatorNetworkServiceErrorCallback>, Vector<String>&);

    Document* document() const;
    Page* page() const;

    void startRequest(ScriptExecutionContext*, PassRefPtr<NavigatorNetworkServiceSuccessCallback>, PassRefPtr<NavigatorNetworkServiceErrorCallback>, Vector<String>&);

    ScriptExecutionContext* m_scriptExecutionContext;
    NetworkServicesController* m_networkServicesController;

    Vector<RefPtr<NetworkServicesRequest> > m_requests;
    RefPtr<NetworkServicesRequest> m_currentRequest;
};

} // namespace WebCore

#endif

#endif // NetworkServicesManager_h
