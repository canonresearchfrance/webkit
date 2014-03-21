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

#ifndef WebNetworkServicesManager_h
#define WebNetworkServicesManager_h

#if ENABLE(DISCOVERY)

#include "MessageReceiver.h"

#include "NetworkServicesRequest.h"
#include "WebNetworkService.h"
#include "WebProcessSupplement.h"
#include <wtf/Forward.h>
#include <wtf/HashMap.h>
#include <wtf/HashSet.h>
#include <wtf/Noncopyable.h>

namespace WebKit {

class WebPage;
class WebProcess;

class WebNetworkServicesManager : public WebProcessSupplement, public IPC::MessageReceiver {
    WTF_MAKE_NONCOPYABLE(WebNetworkServicesManager);
public:
    explicit WebNetworkServicesManager(WebProcess*);
    ~WebNetworkServicesManager();

    static const char* supplementName();

    void registerWebPage(WebPage*);
    void unregisterWebPage(WebPage*);

    void sendDiscoveredServices();
    void subscribeEvent(const String&);
    
private:
    // IPC::MessageReceiver
    virtual void didReceiveMessage(IPC::Connection*, IPC::MessageDecoder&);

    void didReceiveNetworkServiceChanged(WebNetworkService&);
    void didReceiveNetworkServiceEvent(WebNetworkService&, String event);
    void didReceiveDispatchExistingNetworkService(WebNetworkService&);
    void didReceiveNotifyDiscoveryFinished();

    WebProcess* m_process;
    HashSet<WebPage*> m_pageSet;
};

} // namespace WebKit

#endif // ENABLE(DISCOVERY)

#endif // WebNetworkServicesManager_h
