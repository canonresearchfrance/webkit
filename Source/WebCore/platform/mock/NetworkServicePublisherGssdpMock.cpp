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
#include "NetworkServicePublisherGssdpMock.h"

#if ENABLE(DISCOVERY)

#include "Logging.h"
#include <gio/gio.h>
#include <libgssdp/gssdp.h>
#include <stdio.h>
#include <string.h>
#include <wtf/gobject/GRefPtr.h>

namespace WebCore {

Vector<ServiceInfoGssdpMock*> NetworkServicePublisherGssdpMock::m_services;

class ServiceInfoGssdpMock {
public:
    ServiceInfoGssdpMock();
    ~ServiceInfoGssdpMock();

    char* m_name;
    char* m_stype;
    char* m_txt;
    char* m_host;

    guint m_resourceId;

    GRefPtr<GSSDPClient> m_ssdpClient;
    GRefPtr<GSSDPResourceGroup> m_resourceGroup;
};

ServiceInfoGssdpMock::ServiceInfoGssdpMock()
    : m_name(0)
    , m_stype(0)
    , m_txt(0)
    , m_host(0)
    , m_ssdpClient(0)
    , m_resourceGroup(0)
{
}

ServiceInfoGssdpMock::~ServiceInfoGssdpMock()
{
    gssdp_resource_group_remove_resource(m_resourceGroup.get(), m_resourceId);
    free(m_name);
    free(m_stype);
    free(m_txt);    
    free(m_host);
}


void NetworkServicePublisherGssdpMock::addService(const char* name, const char* stype, const char* txt, const char* host)
{
    ServiceInfoGssdpMock* si = new ServiceInfoGssdpMock;

    if (!si)
        return;

    si->m_name  = strdup(name);
    si->m_stype = strdup(stype);
    si->m_txt   = strdup(txt);
    if (strcmp(host, "null"))
        si->m_host  = strdup(host);

    /* Create a new GssdpClient instance */
    si->m_ssdpClient = adoptGRef(gssdp_client_new(0, 0, 0));
    if (!si->m_ssdpClient)
        return;

    si->m_resourceGroup = adoptGRef(gssdp_resource_group_new(si->m_ssdpClient.get()));
    
    si->m_resourceId = gssdp_resource_group_add_resource_simple(si->m_resourceGroup.get(),
        "upnp:rootdevice",
        "uuid:1234abcd-12ab-12ab-12ab-1234567abc12::upnp:rootdevice",
        si->m_host ? si->m_host : "http://127.0.0.1");

    gssdp_resource_group_set_available(si->m_resourceGroup.get(), TRUE);

    m_services.append(si);
}

void NetworkServicePublisherGssdpMock::removeService(const char* id)
{
    for (size_t i = 0; i < m_services.size(); i++) {
        ServiceInfoGssdpMock* si = m_services[i];
        char* siId = 0;

        // sprintf(siId, "%s.%s.local.", si->m_name, si->m_stype);

        if (!strcmp(siId, id)) {
            printf("Remove mock service : name %s, stype %s, host %s\n", 
                si->m_name, 
                si->m_stype, 
                si->m_host);
            m_services.remove(i);
            delete si;
            break;
        }
    }
}

void NetworkServicePublisherGssdpMock::removeAllServices()
{
    for (size_t i = 0; i < m_services.size(); i++)
        delete m_services[i];
    m_services.clear();
}

} // namespace WebCore

#endif // ENABLE(DISCOVERY)
