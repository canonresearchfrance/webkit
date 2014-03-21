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
#include "NetworkServicePublisherAvahiMock.h"

#if ENABLE(DISCOVERY)

#include "Logging.h"
#include <avahi-client/client.h>
#include <avahi-client/publish.h>
#include <avahi-common/alternative.h>
#include <avahi-common/error.h>
#include <avahi-common/malloc.h>
#include <avahi-glib/glib-malloc.h>
#include <avahi-glib/glib-watch.h>
#include <stdio.h>

namespace WebCore {

Vector<ServiceInfoAvahiMock*> NetworkServicePublisherAvahiMock::m_services;

class ServiceInfoAvahiMock {
public:
    ServiceInfoAvahiMock();
    ~ServiceInfoAvahiMock();

    // Avahi client
    static void avahiClientCallback(AvahiClient*, AvahiClientState, void*);
    static void entryGroupCallback(AvahiEntryGroup*, AvahiEntryGroupState, void*);

    void registerService();

    char* m_name;
    char* m_stype;
    AvahiStringList* m_txt;
    char* m_host;

    AvahiGLibPoll* m_glibPoll;
    AvahiClient* m_avahiClient;
    AvahiEntryGroup* m_entryGroup;
};

ServiceInfoAvahiMock::ServiceInfoAvahiMock()
    : m_name(0)
    , m_stype(0)
    , m_txt(0)
    , m_host(0)
    , m_glibPoll(0)
    , m_avahiClient(0)
    , m_entryGroup(0)
{
}

ServiceInfoAvahiMock::~ServiceInfoAvahiMock()
{
    if (m_entryGroup) {
        avahi_entry_group_free(m_entryGroup);
        m_entryGroup = 0;
    }

    if (m_avahiClient) {
        avahi_client_free(m_avahiClient);
        m_avahiClient = 0;
    }

    if (m_glibPoll) {
        avahi_glib_poll_free(m_glibPoll);
        m_glibPoll = 0;
    }

    avahi_free(m_name);
    avahi_free(m_stype);
    avahi_string_list_free(m_txt);    
    avahi_free(m_host);
}

void ServiceInfoAvahiMock::avahiClientCallback(AvahiClient* client, AvahiClientState state, void* userdata)
{
    ServiceInfoAvahiMock* si = static_cast<ServiceInfoAvahiMock*>(userdata);

    si->m_avahiClient = client;

    switch (state) {
    case AVAHI_CLIENT_FAILURE:
        if (avahi_client_errno(client) == AVAHI_ERR_DISCONNECTED) {
            int error;

            /* We have been disconnected, so let reconnect */

            LOG_ERROR("Disconnected, reconnecting ...\n");

            avahi_client_free(client);
            si->m_avahiClient = 0;
            si->m_entryGroup = 0;

            if (!(client = avahi_client_new(avahi_glib_poll_get(si->m_glibPoll), AVAHI_CLIENT_NO_FAIL, avahiClientCallback, static_cast<void*>(si), &error)))
                LOG_ERROR("Failed to create client object: %s\n", avahi_strerror(error));

        } else
            LOG_ERROR("Client failure, exiting: %s\n", avahi_strerror(avahi_client_errno(client)));

        break;

    case AVAHI_CLIENT_S_RUNNING:
        si->registerService(); 
        break;

    case AVAHI_CLIENT_S_REGISTERING:
        if (si->m_entryGroup) {
            avahi_entry_group_free(si->m_entryGroup);
            si->m_entryGroup = 0;
        }
        break;

    case AVAHI_CLIENT_S_COLLISION:
        break;

    case AVAHI_CLIENT_CONNECTING:
        break;
    }
}

void ServiceInfoAvahiMock::registerService()
{
    if (!m_entryGroup) {
        if (!(m_entryGroup = avahi_entry_group_new(m_avahiClient, entryGroupCallback, static_cast<void*>(this)))) {
            LOG_ERROR("Failed to create entry group: %s\n", avahi_strerror(avahi_client_errno(m_avahiClient)));
            return;
        }
    }

    if (avahi_entry_group_add_service_strlst(m_entryGroup, AVAHI_IF_UNSPEC, AVAHI_PROTO_UNSPEC, (AvahiPublishFlags)0, 
        m_name,
        m_stype,
        0, // m_domain
        m_host,
        80, // m_port 
        m_txt) < 0) {
        LOG_ERROR("Failed to add service: %s\n", avahi_strerror(avahi_client_errno(m_avahiClient)));
        return;

    }

    avahi_entry_group_commit(m_entryGroup);
}

void ServiceInfoAvahiMock::entryGroupCallback(AvahiEntryGroup*, AvahiEntryGroupState state, void *userdata) 
{
    ServiceInfoAvahiMock* si = static_cast<ServiceInfoAvahiMock*>(userdata);

    ASSERT(si);

    switch (state) {

    case AVAHI_ENTRY_GROUP_ESTABLISHED:

        // LOG_ERROR("Established under name '%s'\n", si->m_name);
        break;

    case AVAHI_ENTRY_GROUP_FAILURE:

        LOG_ERROR("Failed to register: %s\n", avahi_strerror(avahi_client_errno(si->m_avahiClient)));
        break;

    case AVAHI_ENTRY_GROUP_COLLISION: {
        char* n;

        n = avahi_alternative_service_name(si->m_name);

        LOG_ERROR("Name collision, picking new name '%s'.\n", n);
        avahi_free(si->m_name);
        si->m_name = n;

        si->registerService();

        break;
    }

    case AVAHI_ENTRY_GROUP_UNCOMMITED:
    case AVAHI_ENTRY_GROUP_REGISTERING:
        break;
    }
}

void NetworkServicePublisherAvahiMock::addService(const char* name, const char* stype, const char* txt, const char* host)
{
    const AvahiPoll* pollApi;
    int error;    
    ServiceInfoAvahiMock* si = new ServiceInfoAvahiMock;

    if (!si)
        return;

    si->m_name  = avahi_strdup(name);
    si->m_stype = avahi_strdup(stype);
    si->m_txt   = avahi_string_list_add(si->m_txt, txt);
    if (strcmp(host, "null"))
        si->m_host  = avahi_strdup(host);

    si->m_glibPoll = avahi_glib_poll_new(0, G_PRIORITY_DEFAULT);
    pollApi = avahi_glib_poll_get(si->m_glibPoll);

    /* Create a new AvahiClient instance */
    si->m_avahiClient = avahi_client_new(pollApi, (AvahiClientFlags)0, ServiceInfoAvahiMock::avahiClientCallback, static_cast<void*>(si), &error);

    /* Check the error return code */
    if (!si->m_avahiClient) {
        /* Print out the error string */
        g_warning("Error initializing Avahi: %s", avahi_strerror(error));

        avahi_glib_poll_free(si->m_glibPoll);
    }

    m_services.append(si);
}

void NetworkServicePublisherAvahiMock::removeService(const char* id)
{
    for (size_t i = 0; i < m_services.size(); i++) {
        ServiceInfoAvahiMock* si = m_services[i];
        char* siId = (char*) avahi_malloc(strlen(si->m_name) + strlen(si->m_stype) + strlen("local") + 4);
        sprintf(siId, "%s.%s.local.", si->m_name, si->m_stype);

        if (!strcmp(siId, id)) {
            m_services.remove(i);
            delete si;
            break;
        }
    }
}

void NetworkServicePublisherAvahiMock::removeAllServices()
{
    for (size_t i = 0; i < m_services.size(); i++)
        delete m_services[i];
    m_services.clear();
}

} // namespace WebCore

#endif // ENABLE(DISCOVERY)
