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

#ifndef NetworkServicesProviderAvahi_h
#define NetworkServicesProviderAvahi_h

#if ENABLE(DISCOVERY)

#include "NetworkServicesProviderBase.h"
#include "Timer.h"
#include <avahi-client/client.h>
#include <avahi-client/lookup.h>
#include <avahi-common/domain.h>
#include <avahi-common/error.h>
#include <avahi-common/llist.h>
#include <avahi-common/malloc.h>
#include <avahi-glib/glib-malloc.h>
#include <avahi-glib/glib-watch.h>

namespace WebCore {

class NetworkServicesProviderClient;
class NetworkServicesProviderAvahi;

typedef struct _ServiceInfo ServiceInfo;

struct _ServiceInfo {
    AvahiIfIndex interface;
    AvahiProtocol protocol;
    char* name;
    char* type;
    char *domain;
    AvahiServiceResolver* resolver;
    NetworkServicesProviderAvahi *provider;
    bool notify;
    
    AVAHI_LLIST_FIELDS(struct _ServiceInfo, info);
};

class NetworkServicesProviderAvahi : public NetworkServicesProviderBase {
public:
    NetworkServicesProviderAvahi(NetworkServicesProviderClient *);

    void startUpdating();
    void stopUpdating();

private: 
    // Avahi client
    static void avahiClientCallback(AvahiClient*, AvahiClientState, void*);
    static void serviceTypeBrowserCallback(
        AvahiServiceTypeBrowser*, 
        AvahiIfIndex, 
        AvahiProtocol, 
        AvahiBrowserEvent, 
        const char*, 
        const char*, 
        AvahiLookupResultFlags, 
        void*);
    static void serviceBrowserCallback(
        AvahiServiceBrowser*, 
        AvahiIfIndex, 
        AvahiProtocol, 
        AvahiBrowserEvent, 
        const char*, 
        const char*, 
        const char*, 
        AvahiLookupResultFlags, 
        void*);
    static void serviceResolverCallback(
        AvahiServiceResolver*,
        AvahiIfIndex,
        AvahiProtocol,
        AvahiResolverEvent,
        const char*,
        const char*,
        const char*,
        const char*,
        const AvahiAddress*,
        uint16_t,
        AvahiStringList*,
        AvahiLookupResultFlags,
        void*);
    
    void browseServiceType(const char*, const char*);
    
    ServiceInfo* findService(AvahiIfIndex, AvahiProtocol, const char*, const char*, const char*);
    ServiceInfo* addServiceInfo(AvahiIfIndex, AvahiProtocol, const char*, const char*, const char*);
    void removeServiceInfo(ServiceInfo*);

    void checkTerminate();

    AvahiGLibPoll* m_glibPoll;
    AvahiClient* m_avahiClient;
    AvahiServiceTypeBrowser* m_avahiBrowser;
    AvahiStringList* m_browsedTypes;

    int m_resolving;
    int m_allForNow;

    ServiceInfo* m_services;
    ServiceInfo* m_currentService;
};

} // namespace WebCore

#endif // ENABLE(DISCOVERY)

#endif // NetworkServicesProviderAvahi_h
