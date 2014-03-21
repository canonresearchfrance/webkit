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

#ifndef NetworkServicesRequestManager_h
#define NetworkServicesRequestManager_h

#if ENABLE(DISCOVERY)

#include "WebNetworkServices.h"
#include <wtf/HashMap.h>
#include <wtf/RefPtr.h>

namespace WebCore {
class NetworkServicesRequest;
}

namespace WebKit {

class WebPage;

class NetworkServicesRequestManager {
public:
    explicit NetworkServicesRequestManager(WebPage*);

    void discoveryRequestStarted(WebCore::NetworkServicesRequest*);
    void discoveryRequestFinished();
    void discoveryRequestUpdated(WebCore::NetworkServicesRequest*) const;
    void discoveryRequestCanceled(WebCore::NetworkServicesRequest*);

    void getNetworkServices(uint64_t requestID, WebNetworkServices::Data&);

    void didReceiveNetworkServiceAllowance(uint64_t requestID, const String& serviceID, bool allowed);
    void didReceiveNetworkServicesPermissionDecision(uint64_t requestID, bool allowed);

private:
    typedef HashMap<uint64_t, WebCore::NetworkServicesRequest*> IDToRequestMap;
    typedef HashMap<WebCore::NetworkServicesRequest*, uint64_t> RequestToIDMap;
    RequestToIDMap m_requestToIDMap;
    IDToRequestMap m_idToRequestMap;

    WebPage* m_page;
};

} // namespace WebKit

#endif // ENABLE(DISCOVERY)

#endif // NetworkServicesRequestManager_h
