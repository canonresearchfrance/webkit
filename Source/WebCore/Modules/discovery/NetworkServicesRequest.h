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

#ifndef NetworkServicesRequest_h
#define NetworkServicesRequest_h

#if ENABLE(DISCOVERY)

#include "NavigatorNetworkServiceErrorCallback.h"
#include "NavigatorNetworkServiceSuccessCallback.h"
#include "NetworkServices.h"
#include "NetworkServicesManager.h"
#include <wtf/PassRefPtr.h>
#include <wtf/RefPtr.h>
#include <wtf/Vector.h>

namespace WebCore {

class NavigatorNetworkServiceSuccessCallback;
class NavigatorNetworkServiceErrorCallback;
class NetworkServicesManager;
class ScriptExecutionContext;

class NetworkServicesRequest : public RefCounted<NetworkServicesRequest> {
public:
    static PassRefPtr<NetworkServicesRequest> create(NetworkServicesManager* client, ScriptExecutionContext* context, PassRefPtr<NavigatorNetworkServiceSuccessCallback> successCallback, PassRefPtr<NavigatorNetworkServiceErrorCallback> errorCallback, Vector<String>& types) 
    { 
        return adoptRef(new NetworkServicesRequest(client, context, successCallback, errorCallback, types)); 
    }
        
    ~NetworkServicesRequest();

    using RefCounted<NetworkServicesRequest>::ref;
    using RefCounted<NetworkServicesRequest>::deref;

    NetworkServicesManager* client() const { return m_client; }
    
    PassRefPtr<NetworkServices> networkServices() const { return m_networkServices; }

    void networkServiceInitialized();
    void networkServiceChanged();
    void networkServiceNotifyEvent();

    void discoveryFinished() { m_discoveryFinished = true; }

    void resetWhiteList() const;

    void setAllowed();
    void setDenied();

private:

    explicit NetworkServicesRequest(NetworkServicesManager*, ScriptExecutionContext*, PassRefPtr<NavigatorNetworkServiceSuccessCallback>, PassRefPtr<NavigatorNetworkServiceErrorCallback>, Vector<String>&);

    NetworkServiceDescription* lastNetworkServiceDescription();
    NotifyEvent* lastNotifyEvent();

    bool containsType(const String& type) const;
    void addService(NetworkServiceDescription*);

    void runSuccessCallback();
    void runErrorCallback(NavigatorNetworkServiceError*);

    void registerWhiteList() const;

    NetworkServicesManager* m_client;

    ScriptExecutionContext *m_scriptExecutionContext;
    RefPtr<NavigatorNetworkServiceSuccessCallback> m_successCallback;
    RefPtr<NavigatorNetworkServiceErrorCallback> m_errorCallback;
    Vector<String> m_types;
    
    bool m_discoveryFinished;
    RefPtr<NetworkServices> m_networkServices;
    RefPtr<NetworkServices> m_dispatchNetworkServices;

    enum {
        InProgress,
        Finished
    } m_requestStatus;

};

} // namespace WebCore

#endif

#endif // NetworkServicesRequest_h
