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

#ifndef NetworkServicesRequestProxy_h
#define NetworkServicesRequestProxy_h

#include "APIObject.h"
#include "WebNetworkServices.h"
#include <wtf/PassRefPtr.h>
#include <wtf/text/WTFString.h>

namespace WebKit {

class NetworkServicesRequestManagerProxy;

class NetworkServicesRequestProxy : public API::ObjectImpl<API::Object::Type::NetworkServicesRequest> {
public:
    static PassRefPtr<NetworkServicesRequestProxy> create(NetworkServicesRequestManagerProxy* manager, uint64_t requestID) 
    {
        return adoptRef(new NetworkServicesRequestProxy(manager, requestID));
    }

    WebNetworkServices* webNetworkServices();

    unsigned long length();
    unsigned long availableServices();
    String origin();
    const WebNetworkService* getServiceByIndex(unsigned long index);

    void setServiceAllowed(const String& serviceId, bool allowed);

    void allow();
    void deny();

    void invalidate();
    
private:
    NetworkServicesRequestProxy(NetworkServicesRequestManagerProxy*, uint64_t requestID);

    NetworkServicesRequestManagerProxy* m_manager;
    uint64_t m_requestID;
    bool m_valid;
    RefPtr<WebNetworkServices> m_webNetworkServices;
};

} // namespace WebKit

#endif // NetworkServicesRequestProxy_h
