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

#ifndef WebNetworkServicesClient_h
#define WebNetworkServicesClient_h

#if ENABLE(DISCOVERY)

// #define DO_NOT_USE_GUPNP_LIB

#include "NetworkService.h"
#include "NetworkServicesClient.h"
#include "NetworkServicesProviderAvahi.h"
#include "NetworkServicesProviderClient.h"
#ifdef DO_NOT_USE_GUPNP_LIB
#include "NetworkServicesProviderGssdp.h"
#else
#include "NetworkServicesProviderGupnp.h"
#endif
#include "NetworkServicesRequest.h"
#include <wtf/PassRefPtr.h>
#include <wtf/text/AtomicString.h>

namespace WebKit {

class WebPage;

class WebNetworkServicesClient : public WebCore::NetworkServicesClient, public WebCore::NetworkServicesProviderClient {
public:
    explicit WebNetworkServicesClient(WebPage*);
    virtual ~WebNetworkServicesClient();
    

    // NetworkServicesClient interface.
    virtual void networkServicesDestroyed();
    virtual void startUpdating();
    virtual void stopUpdating();
    virtual void initializeRequest(PassRefPtr<WebCore::NetworkServicesRequest>);
    virtual void requestUpdated(PassRefPtr<WebCore::NetworkServicesRequest>);
    virtual void requestCanceled(PassRefPtr<WebCore::NetworkServicesRequest>);
    virtual void subscribeEvent(const String&);

private:
    virtual void notifyDiscoveryStarted(PassRefPtr<WebCore::NetworkServicesRequest>);

    // NetworkServicesProviderClient interface.
    virtual void notifyDiscoveryFinished();
    virtual void notifyNetworkServiceChanged(WebCore::NetworkServiceDescription*);
    virtual void notifyNetworkServiceEvent(WebCore::NetworkServiceDescription*, const String&);
    virtual void dispatchExistingNetworkService(WebCore::NetworkServiceDescription*);

    WebPage* m_page;
};

} // namespace WebKit

#endif // ENABLE(DISCOVERY)

#endif // WebNetworkServicesClient_h
