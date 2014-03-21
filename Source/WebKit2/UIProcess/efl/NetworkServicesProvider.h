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

#ifndef NetworkServicesProvider_h
#define NetworkServicesProvider_h

#if ENABLE(DISCOVERY)

#include "NetworkServicesProviderClient.h"
#include "NetworkServicesProviderEfl.h"
#include <WebKit2/WKBase.h>
#include <WebKit2/WKRetainPtr.h>
#include <wtf/PassRefPtr.h>

namespace WebKit {

class NetworkServicesProvider : public RefCounted<NetworkServicesProvider>, public WebCore::NetworkServicesProviderClient {
public:
    virtual ~NetworkServicesProvider();
    static PassRefPtr<NetworkServicesProvider> create(WKContextRef);

    void startUpdating();
    void stopUpdating();
    void sendDiscoveredServices();
    void subscribeEvent(const String&);

private:
    explicit NetworkServicesProvider(WKContextRef);

    // NetworkServicesProviderClient interface.
    virtual void notifyDiscoveryFinished();
    virtual void notifyNetworkServiceChanged(WebCore::NetworkServiceDescription*);
    virtual void notifyNetworkServiceEvent(WebCore::NetworkServiceDescription*, const String&);
    virtual void dispatchExistingNetworkService(WebCore::NetworkServiceDescription*);

    WKRetainPtr<WKNetworkServicesManagerRef> m_manager;
    WebCore::NetworkServicesProviderEfl m_provider;
};

} // namespace WebKit

#endif // ENABLE(DISCOVERY)

#endif // NetworkServicesProvider_h
