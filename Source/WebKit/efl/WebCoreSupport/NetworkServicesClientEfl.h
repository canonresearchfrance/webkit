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

#ifndef NetworkServicesClientEfl_h
#define NetworkServicesClientEfl_h

#if ENABLE(DISCOVERY)

#include "NetworkServiceDescription.h"
#include "NetworkServicesClient.h"
#include "NetworkServicesProviderClient.h"
#include "NetworkServicesProviderEfl.h"
#include "NetworkServicesRequest.h"
#include <Evas.h>
#include <wtf/PassRefPtr.h>
#include <wtf/text/AtomicString.h>

class NetworkServicesClientEfl : public WebCore::NetworkServicesClient, public WebCore::NetworkServicesProviderClient {
public:
    explicit NetworkServicesClientEfl(Evas_Object* view, WebCore::Page*);
    virtual ~NetworkServicesClientEfl();
    
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

    Evas_Object* m_view;
    WebCore::Page* m_page;
    WebCore::NetworkServicesProviderEfl m_provider;
};

#endif // ENABLE(DISCOVERY)

#endif // NetworkServicesClientEfl_h
