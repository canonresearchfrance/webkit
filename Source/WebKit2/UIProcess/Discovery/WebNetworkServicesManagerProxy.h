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

#ifndef WebNetworkServicesManagerProxy_h
#define WebNetworkServicesManagerProxy_h

#if ENABLE(DISCOVERY)

#include "APIObject.h"
#include "MessageReceiver.h"
#include "WKNetworkServicesManager.h"
#include "WebContextSupplement.h"
#include "WebNetworkServicesProvider.h"
#include <wtf/HashMap.h>
#include <wtf/PassRefPtr.h>
#include <wtf/text/AtomicString.h>

namespace WebKit {

class WebNetworkService;

class WebNetworkServicesManagerProxy : public API::ObjectImpl<API::Object::Type::NetworkServicesManager>, public WebContextSupplement, private IPC::MessageReceiver {
public:
    static const char* supplementName();

    static PassRefPtr<WebNetworkServicesManagerProxy> create(WebContext*);
    ~WebNetworkServicesManagerProxy();

    void initializeProvider(const WKNetworkServicesProviderBase*);

    void didReceiveNetworkServiceChanged(WebNetworkService*);
    void didReceiveNetworkServiceEvent(WebNetworkService*, const String& event);
    void didReceiveDispatchExistingNetworkService(WebNetworkService*);
    void didReceiveNotifyDiscoveryFinished();

    using API::Object::ref;
    using API::Object::deref;

private:
    explicit WebNetworkServicesManagerProxy(WebContext*);

    // WebContextSupplement
    virtual void contextDestroyed();
    virtual void processDidClose(WebProcessProxy*);
    virtual void refWebContextSupplement();
    virtual void derefWebContextSupplement();
    
    // IPC::MessageReceiver
    virtual void didReceiveMessage(IPC::Connection*, IPC::MessageDecoder&);

    void startUpdating();
    void stopUpdating();
    void sendDiscoveredServices();
    void subscribeEvent(const String& id);    

    bool m_isUpdating;

    WebNetworkServicesProvider m_provider;
};

} // namespace WebKit

#endif // ENABLE(DISCOVERY)

#endif // WebNetworkServicesManagerProxy_h
