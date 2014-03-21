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
#include "NetworkServicesProviderGssdp.h"

#if ENABLE(DISCOVERY)

#include "Logging.h"
#include "NetworkServicesProviderClient.h"
#include <wtf/text/CString.h>
#include <wtf/text/WTFString.h>

namespace WebCore {

NetworkServicesProviderGssdp::NetworkServicesProviderGssdp(NetworkServicesProviderClient *client)
    : NetworkServicesProviderBase(client)
    , m_ssdpClient(0)
    , m_ssdpBrowser(0)
    , m_timer(this, &NetworkServicesProviderGssdp::timerFired)
{
    m_ssdpResolver = SSDPResolver::create(this);
}

void NetworkServicesProviderGssdp::startUpdating()
{
    if (m_ssdpClient)
        return;

    m_ssdpClient = adoptGRef(gssdp_client_new(0, 0, 0));
    if (!m_ssdpClient)
        return;

    m_ssdpBrowser = adoptGRef(gssdp_resource_browser_new(m_ssdpClient.get(), GSSDP_ALL_RESOURCES));

    g_signal_connect(m_ssdpBrowser.get(), "resource-available", G_CALLBACK(ssdpServiceAvailable), static_cast<void*>(this));
    g_signal_connect(m_ssdpBrowser.get(), "resource-unavailable", G_CALLBACK(ssdpServiceUnavailable), static_cast<void*>(this));

    gssdp_resource_browser_set_active(m_ssdpBrowser.get(), TRUE);

    m_timer.startOneShot(3);
}

void NetworkServicesProviderGssdp::stopUpdating()
{
    m_ssdpBrowser.clear();
    m_ssdpClient.clear();

    m_timer.stop();
}

void NetworkServicesProviderGssdp::timerFired(Timer<NetworkServicesProviderGssdp>*)
{
    m_timer.stop();

    // Parse all descrition URI retreive
    m_ssdpResolver->parseDescription();

    notifyDiscoveryFinished();
}

bool NetworkServicesProviderGssdp::updateServiceDescription(const char* serviceId, const char* controlUrl, const char* config, const char* eventUrl)
{
    NetworkServiceDescription* desc = getServiceDescriptionById(String(serviceId));

    if (desc) {
        desc->update(String(controlUrl), String(config));
        desc->setEventSubURL(String(eventUrl));
        notifyNetworkServiceChanged(desc);
        return true;
    }
    return false;
}
void NetworkServicesProviderGssdp::ssdpServiceAvailable(GSSDPResourceBrowser* browser, char *usn, gpointer locations, gpointer data)
{
    GList* locs = (GList*)locations;
    char* notificationType;
    String type;
    NetworkServicesProviderGssdp* p = static_cast<NetworkServicesProviderGssdp*>(data);
    Vector<NetworkServiceDescription*> descList;

    UNUSED_PARAM(browser);

    descList = p->getDescriptionListFromUsn(usn, &notificationType);
    if (descList.size()) {

        for (size_t i = 0; i < descList.size(); i++) {        
            NetworkServiceDescription* desc = descList[i];
            LOG(Network, "+ %-60s %20s %s", 
                desc->id().utf8().data(), 
                desc->type().utf8().data(), 
                desc->name().utf8().data());
            desc->setOnline(true);

            while (locs) {
                LOG(Network, "+ %s", usn);
                p->m_ssdpResolver->resolve((const char*)locs->data);
                
                locs = g_list_next(locs);
            }
        }
    } else if (notificationType && !strcmp(notificationType, "upnp:rootdevice")) {
        // Rearm timer  
        // p->m_timer.stop();
        // p->m_timer.startOneShot(1);

        while (locs) {
            LOG(Network, "+ %s", usn);
            p->m_ssdpResolver->resolve((const char*)locs->data);
                
            locs = g_list_next(locs);
        }
    }
}

void NetworkServicesProviderGssdp::ssdpServiceUnavailable(GSSDPResourceBrowser *browser, char *usn, gpointer data)
{
    UNUSED_PARAM(browser);

    char* notificationType;
    NetworkServicesProviderGssdp* p = static_cast<NetworkServicesProviderGssdp*>(data);
    Vector<NetworkServiceDescription*> descList = p->getDescriptionListFromUsn(usn, &notificationType);

    if (descList.size()) {

        for (size_t i = 0; i < descList.size(); i++) {        
            NetworkServiceDescription* desc = descList[i];
            LOG(Network, "- %-60s %20s %s", 
                desc->id().utf8().data(), 
                desc->type().utf8().data(), 
                desc->name().utf8().data());
            desc->setOnline(false);
            p->notifyNetworkServiceChanged(desc);
        }
    } else
        p->notifyDiscoveryFinished();
}

Vector<NetworkServiceDescription*> NetworkServicesProviderGssdp::getDescriptionListFromUsn(const char* usn, char** notificationType) const
{
    char** usnTokens;
    char* uuid;
    String type;

    usnTokens = g_strsplit(usn, "::", -1);
    g_assert(usnTokens && usnTokens[0]);
    
    if (!usnTokens[1])
        return Vector<NetworkServiceDescription*>(0);

    uuid = usnTokens[0];
    *notificationType = usnTokens[1];

    /* retreive all services which:
     * - the "id" startWith uuid 
     * - the "type" match with "upnp:" + notificationType
     */
    type = String::fromUTF8("upnp:");
    type.append(String::fromUTF8(*notificationType));
    return getDescriptionListByUuidAndType(String::fromUTF8(uuid), type);
}

Vector<NetworkServiceDescription*> NetworkServicesProviderGssdp::getDescriptionListByUuidAndType(const String& uuid, const String& type) const
{
    Vector<NetworkServiceDescription*> descList;

    for (size_t i = 0; i < m_descriptions.size(); i++) {
        NetworkServiceDescription* desc = m_descriptions[i];

        if (desc->id().startsWith(uuid) && desc->type() == type)
            descList.append(desc);
    }

    return descList;
}
} // namespace WebCore

#endif // ENABLE(DISCOVERY)
