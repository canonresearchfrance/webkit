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

#include "config.h"
#include "NetworkServicesProviderAvahi.h"

#if ENABLE(DISCOVERY)

#include "Logging.h"
#include "NetworkServicesProviderBase.h"
#include "NetworkServicesProviderClient.h"
#include <avahi-common/domain.h>
#include <wtf/ASCIICType.h>
#include <wtf/text/CString.h>
#include <wtf/text/WTFString.h>

namespace WebCore {

#ifdef DEBUG
static char* makePrintable(const char *from, char *to) 
{
    const char* f;
    char* t;

    for (f = from, t = to; *f; f++, t++)
        *t = isASCIIPrintable(*f) ? *f : '_';

    *t = 0;

    return to;
}

static void printServiceLine(
    char c, 
    AVAHI_GCC_UNUSED AvahiIfIndex iface, 
    AvahiProtocol proto, 
    const char* name, 
    const char* type, 
    const char* domain) 
{
    char label[AVAHI_LABEL_MAX];
    const char* protoStr;
    
    UNUSED();

    makePrintable(name, label);
    switch (proto) {
    case AVAHI_PROTO_INET:
        protoStr = "IPv4"; 
        break;

    case AVAHI_PROTO_INET6:
        protoStr = "IPv6"; 
        break;

    case AVAHI_PROTO_UNSPEC:
    default:
        protoStr = "Unknown"; 
        break;
    }

    LOG(Network, "%c (%s) %-60s %-20s %s\n", c, protoStr, label, type, domain);
}
#else
#define printServiceLine(...)
#endif

NetworkServicesProviderAvahi::NetworkServicesProviderAvahi(NetworkServicesProviderClient *client)
    : NetworkServicesProviderBase(client)
    , m_glibPoll(0)
    , m_avahiClient(0)
    , m_avahiBrowser(0)
    , m_browsedTypes(0)
    , m_resolving(0)
    , m_allForNow(0)
    , m_services(0)
    , m_currentService(0)
{
}

ServiceInfo* NetworkServicesProviderAvahi::findService(
    AvahiIfIndex interface,
    AvahiProtocol protocol,
    const char* name,
    const char* type,
    const char* domain) 
{
    ServiceInfo* i;

    for (i = m_services; i; i = i->info_next) {
        // TODO : Need to deal case of several interface 
        if (i->interface == interface
            && !strcasecmp(i->name, name) 
            && avahi_domain_equal(i->type, type) 
            && avahi_domain_equal(i->domain, domain)) {
            // check protocol IPv4 / IPv6
            if (i->protocol == protocol)
                i->notify = true;
            return i;
        }
    }

    return 0;
}

void NetworkServicesProviderAvahi::serviceResolverCallback(
    AVAHI_GCC_UNUSED AvahiServiceResolver* r,
    AVAHI_GCC_UNUSED AvahiIfIndex interface,
    AVAHI_GCC_UNUSED AvahiProtocol protocol,
    AvahiResolverEvent event,
    const char* name,
    const char* type,
    const char* domain,
    const char* hostName,
    const AvahiAddress* a,
    AVAHI_GCC_UNUSED uint16_t port,
    AvahiStringList* txt,
    AVAHI_GCC_UNUSED AvahiLookupResultFlags flags,
    void* userdata) 
{
    ServiceInfo* i = static_cast<ServiceInfo*>(userdata);
    NetworkServicesProviderAvahi* p = i->provider;

    ASSERT(r);
    ASSERT(i);

    switch (event) {
    case AVAHI_RESOLVER_FOUND: 
        {
            NetworkServiceDescription* desc;
            char address[AVAHI_ADDRESS_STR_MAX];
            char* id;
            char* ztype;
            char* url;
            char* config;

            avahi_address_snprint(address, sizeof(address), a);

            config = avahi_string_list_to_string(txt);

            printServiceLine('=', interface, protocol, name, type, domain);

            LOG(Network,
                "   hostname = [%s]\n"
                "   address = [%s]\n"
                "   port = [%u]\n"
                "   txt = [%s]\n",
                hostName,
                address,
                port,
                config);
    
            id = (char*) avahi_malloc(strlen(name) + strlen(type) + strlen(domain) + 4);
            sprintf(id, "%s.%s.%s.", name, type, domain);

            ztype = (char*) avahi_malloc(strlen(type) + strlen("zeroconf:") + 1);
            sprintf(ztype, "zeroconf:%s", type);

            url = (char*) avahi_malloc(strlen("http://") + strlen(hostName) + 1);
            sprintf(url, "http://%s", hostName);

            if (!(desc = p->getServiceDescriptionById(String::fromUTF8(id))))
                p->addServiceDescription(id, name, ztype, url, config);
            else {
                desc->setOnline(true);
                p->notifyNetworkServiceChanged(desc);
            }

            avahi_free(id);
            avahi_free(ztype);
            avahi_free(url);
            avahi_free(config);

            break;
        }

    case AVAHI_RESOLVER_FAILURE:

        LOG_ERROR("Failed to resolve service '%s' of type '%s' in domain '%s': %s\n", name, type, domain, avahi_strerror(avahi_client_errno(p->m_avahiClient)));
        break;
    }

    avahi_service_resolver_free(i->resolver);
    i->resolver = 0;

    p->m_resolving--;
    p->checkTerminate();
}

ServiceInfo* NetworkServicesProviderAvahi::addServiceInfo(
    AvahiIfIndex interface, 
    AvahiProtocol protocol, 
    const char* name, 
    const char* type, 
    const char* domain) 
{
    ServiceInfo* i;

    m_currentService = i = avahi_new(ServiceInfo, 1);

    if (!(i->resolver = avahi_service_resolver_new(
        m_avahiClient,
        interface, 
        protocol, 
        name, 
        type, 
        domain, 
        AVAHI_PROTO_UNSPEC, 
        (AvahiLookupFlags)0, 
        &serviceResolverCallback, 
        static_cast<void*>(i)))) 
    {
        avahi_free(i);
        LOG_ERROR("Failed to resolve service '%s' of type '%s' in domain '%s': %s\n", 
            name, type, domain, avahi_strerror(avahi_client_errno(m_avahiClient)));
        return 0;
    }

    m_resolving++;

    i->interface = interface;
    i->protocol = protocol;
    i->name = avahi_strdup(name);
    i->type = avahi_strdup(type);
    i->domain = avahi_strdup(domain);
    i->provider = this;
    i->notify = false;

    AVAHI_LLIST_PREPEND(ServiceInfo, info, m_services, i);

    return i;
}

void NetworkServicesProviderAvahi::removeServiceInfo(ServiceInfo* i) 
{
    ASSERT(i);
    NetworkServicesProviderAvahi* p = i->provider;
    char* id;
    
    id = (char*) avahi_malloc(strlen(i->name) + strlen(i->type) + strlen(i->domain) + 4);
    sprintf(id, "%s.%s.%s.", i->name, i->type, i->domain);

    p->removeServiceDescription(id);

    AVAHI_LLIST_REMOVE(ServiceInfo, info, m_services, i);

    if (i->resolver)
        avahi_service_resolver_free(i->resolver);

    avahi_free(i->name);
    avahi_free(i->type);
    avahi_free(i->domain);
    avahi_free(i);
}

void NetworkServicesProviderAvahi::checkTerminate() 
{
    ASSERT(m_allForNow >= 0);
    ASSERT(m_resolving >= 0);

    if (m_allForNow <= 0 && m_resolving <= 0)
        notifyDiscoveryFinished();
}

void NetworkServicesProviderAvahi::serviceBrowserCallback(
    AVAHI_GCC_UNUSED AvahiServiceBrowser* b,
    AvahiIfIndex interface,
    AvahiProtocol protocol,
    AvahiBrowserEvent event,
    const char* name,
    const char* type,
    const char* domain,
    AVAHI_GCC_UNUSED AvahiLookupResultFlags flags,
    void* userdata) 
{
    NetworkServicesProviderAvahi* p = static_cast<NetworkServicesProviderAvahi*>(userdata);

    ASSERT(b);
    ASSERT(p);

    switch (event) {
    case AVAHI_BROWSER_NEW: 
        {
            ServiceInfo* info;

            printServiceLine('+', interface, protocol, name, type, domain);

            if ((info = p->findService(interface, protocol, name, type, domain))) {
                char* id;
                NetworkServiceDescription* desc;
                
                if (!(info->notify))
                    return;

                id = (char*) avahi_malloc(strlen(name) + strlen(type) + strlen(domain) + 4);
                sprintf(id, "%s.%s.%s.", name, type, domain);
                desc = p->getServiceDescriptionById(String::fromUTF8(id));
                if (desc) {
                    desc->setOnline(true);
                    p->notifyNetworkServiceChanged(desc);
                }
                avahi_free(id);
                info->notify = false;

                return;
            }

            p->addServiceInfo(interface, protocol, name, type, domain);

            break;

        }

    case AVAHI_BROWSER_REMOVE: 
        {
            ServiceInfo* info;

            if (!(info = p->findService(interface, protocol, name, type, domain)))
                return;

            if (info->notify) {
                p->removeServiceInfo(info);
                info->notify = false;
            }

            printServiceLine('-', interface, protocol, name, type, domain);
            break;
        }

    case AVAHI_BROWSER_FAILURE:
        LOG_ERROR("service_browser failed: %s\n", avahi_strerror(avahi_client_errno(p->m_avahiClient)));
        break;

    case AVAHI_BROWSER_CACHE_EXHAUSTED:
        break;

    case AVAHI_BROWSER_ALL_FOR_NOW:
        p->m_allForNow--;
        p->checkTerminate();
        break;
    }
}

void NetworkServicesProviderAvahi::browseServiceType(const char* stype, const char* domain)
{
    AvahiServiceBrowser* b;
    AvahiStringList* i;

    ASSERT(stype);

    for (i = m_browsedTypes; i; i = i->next)
        if (avahi_domain_equal(stype, (char*) i->text))
            return;
    
    if (!(b = avahi_service_browser_new(
        m_avahiClient, 
        AVAHI_IF_UNSPEC, 
        AVAHI_PROTO_UNSPEC,
        stype, 
        domain, 
        (AvahiLookupFlags)0, 
        serviceBrowserCallback, 
        static_cast<void*>(this)))) 
    {
        LOG_ERROR("avahi_service_browser_new() failed: %s\n", avahi_strerror(avahi_client_errno(m_avahiClient)));
    }

    m_browsedTypes = avahi_string_list_add(m_browsedTypes, stype);

    m_allForNow++;
}

void NetworkServicesProviderAvahi::serviceTypeBrowserCallback(
    AVAHI_GCC_UNUSED AvahiServiceTypeBrowser* b,
    AVAHI_GCC_UNUSED AvahiIfIndex interface,
    AVAHI_GCC_UNUSED AvahiProtocol protocol,
    AvahiBrowserEvent event,
    const char* type,
    const char* domain,
    AVAHI_GCC_UNUSED AvahiLookupResultFlags flags,
    void* userdata) 
{
    NetworkServicesProviderAvahi* p = static_cast<NetworkServicesProviderAvahi*>(userdata);

    ASSERT(b);
    ASSERT(p);
    ASSERT(p->m_avahiClient);
    
    switch (event) {

    case AVAHI_BROWSER_NEW:
        p->browseServiceType(type, domain);
        break;

    case AVAHI_BROWSER_REMOVE:
        /* We're dirty and never remove the browser again */
        break;

    case AVAHI_BROWSER_FAILURE:
        LOG_ERROR("service_type_browser failed: %s\n", avahi_strerror(avahi_client_errno(p->m_avahiClient)));
        break;

    case AVAHI_BROWSER_CACHE_EXHAUSTED:
        break;

    case AVAHI_BROWSER_ALL_FOR_NOW:
        p->m_allForNow--;
        p->checkTerminate();
        break;
    }


}

void NetworkServicesProviderAvahi::avahiClientCallback(AvahiClient* client, AvahiClientState state, void* userdata)
{
    NetworkServicesProviderAvahi* p = static_cast<NetworkServicesProviderAvahi*>(userdata);

    switch (state) {
    case AVAHI_CLIENT_FAILURE:
        break;

    case AVAHI_CLIENT_S_REGISTERING:
    case AVAHI_CLIENT_S_RUNNING:
    case AVAHI_CLIENT_S_COLLISION:
        p->m_avahiBrowser = avahi_service_type_browser_new(
            client, 
            AVAHI_IF_UNSPEC,
            AVAHI_PROTO_UNSPEC, 
            0, 
            (AvahiLookupFlags)0, 
            serviceTypeBrowserCallback, 
            userdata);
        p->m_allForNow++;
        break;

    case AVAHI_CLIENT_CONNECTING:
        break;
    }
}

void NetworkServicesProviderAvahi::startUpdating()
{
    const AvahiPoll* pollApi;
    int error;    

    if (m_avahiClient)
        return;

    m_glibPoll = avahi_glib_poll_new(0, G_PRIORITY_DEFAULT);
    pollApi = avahi_glib_poll_get(m_glibPoll);

    /* Create a new AvahiClient instance */
    m_avahiClient = avahi_client_new(pollApi, (AvahiClientFlags)0, avahiClientCallback, static_cast<void*>(this), &error);

    /* Check the error return code */
    if (!m_avahiClient) {
        /* Print out the error string */
        g_warning("Error initializing Avahi: %s", avahi_strerror(error));

        avahi_glib_poll_free(m_glibPoll);

        notifyDiscoveryFinished();
    }
}

void NetworkServicesProviderAvahi::stopUpdating()
{
    while (m_services)
        removeServiceInfo(m_services);

    avahi_string_list_free(m_browsedTypes);
    m_browsedTypes = 0;

    // clean all internal fields and counters
    m_avahiBrowser = 0;
    m_allForNow = 0;
    m_resolving = 0;
    m_services = 0;
    m_currentService = 0;

    if (m_avahiClient) {
        avahi_client_free(m_avahiClient);    
        m_avahiClient = 0;
    }

    if (m_glibPoll) {
        avahi_glib_poll_free(m_glibPoll);
        m_glibPoll = 0;
    }
}

} // namespace WebCore

#endif // ENABLE(DISCOVERY)
