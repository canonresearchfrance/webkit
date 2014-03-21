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
#include "NetworkServicesProviderGupnp.h"

#if ENABLE(DISCOVERY)

#include "Logging.h"
#include "NetworkServicesProviderClient.h"
#include <wtf/text/CString.h>

namespace WebCore {

NetworkServicesProviderGupnp::NetworkServicesProviderGupnp(NetworkServicesProviderClient* client)
    : NetworkServicesProviderBase(client)
    , m_context(0)
    , m_controlPoint(0)
    , m_timer(this, &NetworkServicesProviderGupnp::timerFired)
{
}

void NetworkServicesProviderGupnp::startUpdating()
{
    if (m_context)
        return;

    m_context = adoptGRef(gupnp_context_new(0, 0, 0, 0));
    if (!m_context)
        return;

    m_controlPoint = adoptGRef(gupnp_control_point_new(m_context.get(), GSSDP_ALL_RESOURCES));

    g_signal_connect(m_controlPoint.get(), "device-proxy-available", G_CALLBACK(upnpDeviceAvailable), static_cast<void*>(this));
    g_signal_connect(m_controlPoint.get(), "device-proxy-unavailable", G_CALLBACK(upnpDeviceUnavailable), static_cast<void*>(this));
    g_signal_connect(m_controlPoint.get(), "service-proxy-available", G_CALLBACK(upnpServiceAvailable), static_cast<void*>(this));
    g_signal_connect(m_controlPoint.get(), "service-proxy-unavailable", G_CALLBACK(upnpServiceUnavailable), static_cast<void*>(this));

    gssdp_resource_browser_set_active(GSSDP_RESOURCE_BROWSER(m_controlPoint.get()), TRUE);

    m_timer.startOneShot(3);
}

void NetworkServicesProviderGupnp::stopUpdating()
{
    m_context.clear();
    m_controlPoint.clear();   

    m_timer.stop();
}

void NetworkServicesProviderGupnp::timerFired(Timer<NetworkServicesProviderGupnp>*)
{
    m_timer.stop();

    resolveAllPendingDesc();

    notifyDiscoveryFinished();
}

void NetworkServicesProviderGupnp::subscribeEventNotification(const String& id)
{
    NetworkServiceDescriptionGupnp* desc = static_cast<NetworkServiceDescriptionGupnp*>(getServiceDescriptionById(id));

    desc->subscribeEvent(upnpEventNotification, static_cast<void*>(this));
}

void NetworkServicesProviderGupnp::upnpDeviceAvailable(GUPnPControlPoint*, GUPnPDeviceProxy* proxy, gpointer data)
{
    NetworkServicesProviderGupnp* p = static_cast<NetworkServicesProviderGupnp*>(data);
    RefPtr<UPnPDevice> device = UPnPDevice::create(GUPNP_DEVICE_INFO(proxy));

    p->m_devices.add(device->udn(), device);

    p->resolvePendingDesc(device);
}

void NetworkServicesProviderGupnp::upnpDeviceUnavailable(GUPnPControlPoint*, GUPnPDeviceProxy* proxy, gpointer data)
{
    NetworkServicesProviderGupnp* p = static_cast<NetworkServicesProviderGupnp*>(data);
    GUPnPDeviceInfo* di = GUPNP_DEVICE_INFO(proxy);
    String udn = String(gupnp_device_info_get_udn(di));
    
    p->m_devices.remove(udn);
}

void NetworkServicesProviderGupnp::upnpServiceAvailable(GUPnPControlPoint*, GUPnPServiceProxy* proxy, gpointer data)
{
    NetworkServicesProviderGupnp* p = static_cast<NetworkServicesProviderGupnp*>(data);
    GUPnPServiceInfo* si = GUPNP_SERVICE_INFO(proxy);
    char* id = createId(si);
    NetworkServiceDescriptionGupnp* desc = p->getServiceDesc(si, id);
    String udn = String(gupnp_service_info_get_udn(si));
    bool hasDevice = p->m_devices.contains(udn);

    if (desc) {
        LOG(Network, "+ %-60s %20s %s", 
            desc->id().utf8().data(), 
            desc->type().utf8().data(), 
            desc->name().utf8().data());

        desc->setOnline(true);

        if (hasDevice) {
            char* controlUrl = gupnp_service_info_get_control_url(si);
            RefPtr<UPnPDevice> device = p->m_devices.find(udn)->value;
            String config = String(device->config());

            desc->update(String(controlUrl), config);
            g_free(controlUrl);
            
            p->notifyNetworkServiceChanged(desc);
        } else
            p->addPendingDescription(udn, desc);

    } else {
        char* serviceId;
        const char* config = 0;
        char* controlUrl;
        const char* serviceType;
        char* type;
        
        controlUrl = gupnp_service_info_get_control_url(si);
        serviceType = gupnp_service_info_get_service_type(si);

        type = (char*)g_malloc(strlen(serviceType) + 6 /* "upnp:" + '\0' */);
        if (type)
            sprintf(type, "upnp:%s", serviceType);

        serviceId = gupnp_service_info_get_id(si);

        if (hasDevice) {
            RefPtr<UPnPDevice> device = p->m_devices.find(udn)->value;
            config = device->config();
        }

        desc = new NetworkServiceDescriptionGupnp(proxy, id, serviceId, type, controlUrl, config);

        if (type)
            g_free(type);
        g_free(controlUrl);
        g_free(serviceId);

        LOG(Network, "= %s\n"
            "   name = [%s]\n"
            "   type = [%s]\n"
            "   url = [%s]\n",
            id, serviceId, type, controlUrl);

        if (hasDevice)
            p->addServiceDescription(desc);
        else
            p->addPendingDescription(udn, desc);
    }

    g_free(id);
}

void NetworkServicesProviderGupnp::upnpServiceUnavailable(GUPnPControlPoint*, GUPnPServiceProxy* proxy, gpointer data)
{
    NetworkServicesProviderGupnp* p = static_cast<NetworkServicesProviderGupnp*>(data);
    NetworkServiceDescriptionGupnp* desc = p->getServiceDesc(GUPNP_SERVICE_INFO(proxy));

    if (desc) {
        LOG(Network, "- %-60s %20s %s", 
            desc->id().utf8().data(), 
            desc->type().utf8().data(), 
            desc->name().utf8().data());

        desc->setOnline(false);
        p->notifyNetworkServiceChanged(desc);
    } else
        p->notifyDiscoveryFinished();
}

void NetworkServicesProviderGupnp::upnpEventNotification(GUPnPServiceProxy* proxy, const char*, GValue* value, gpointer data)
{
    NetworkServicesProviderGupnp* p = static_cast<NetworkServicesProviderGupnp*>(data);
    NetworkServiceDescriptionGupnp* desc = p->getServiceDesc(GUPNP_SERVICE_INFO(proxy));

    if (desc) {
        xmlDoc* doc = (xmlDoc*)g_value_get_pointer(value);
        xmlBufferPtr configBuf = xmlBufferCreate();
        xmlNode* rootNode = xmlDocGetRootElement(doc);

        if (!configBuf)
            return;

        xmlNodeDump(configBuf, doc, rootNode, 1, 1);

        p->m_client->notifyNetworkServiceEvent(desc, String::fromUTF8(configBuf->content));

        if (configBuf)
            xmlBufferFree(configBuf);
    }
}

inline char* NetworkServicesProviderGupnp::createId(GUPnPServiceInfo *si)
{
    const char* udn;
    char* serviceId;
    char* id;

    udn = gupnp_service_info_get_udn(si);
    serviceId = gupnp_service_info_get_id(si);

    /* Concatenate udn::serviceId */
    id = (char*)g_malloc(strlen(udn) + strlen(serviceId) + 3 /* "::" + '\0' */);
    if (!id)
        return 0;
    
    sprintf(id, "%s::%s", udn, serviceId);

    g_free(serviceId);

    return id;
}

NetworkServiceDescriptionGupnp* NetworkServicesProviderGupnp::getServiceDesc(GUPnPServiceInfo* si, char* id)
{
    NetworkServiceDescriptionGupnp* desc;
    char* tmpId = 0;
    
    if (!id) {
        id = createId(si);
        tmpId = id;
    }

    desc = static_cast<NetworkServiceDescriptionGupnp*>(getServiceDescriptionById(String::fromUTF8(id)));

    if (tmpId)
        g_free(tmpId);

    return desc;
}

void NetworkServicesProviderGupnp::addPendingDescription(String& udn, NetworkServiceDescriptionGupnp* desc)
{
    PendingDesc::iterator it = m_pendingDesc.find(udn);

    if (it == m_pendingDesc.end())
        it = m_pendingDesc.add(udn, Vector<NetworkServiceDescriptionGupnp*>()).iterator;

    it->value.append(desc);
}

void NetworkServicesProviderGupnp::resolveAllPendingDesc()
{
    PendingDesc::iterator end = m_pendingDesc.end();
    
    for (PendingDesc::iterator it = m_pendingDesc.begin(); it != end; ++it) {
        Vector<NetworkServiceDescriptionGupnp*> descriptions = it->value;
        String udn = it->key;
        bool hasDevice = m_devices.contains(udn);

        for (size_t i = 0; i < descriptions.size(); ++i) {
            NetworkServiceDescriptionGupnp* desc = descriptions[i];
            RefPtr<UPnPDevice> device;
            
            if (hasDevice) {
                device = m_devices.find(udn)->value;
                String config = String(device->config());

                desc->update(desc->url(), config);

                addServiceDescription(desc);
            }
        }
    }
}

void NetworkServicesProviderGupnp::resolvePendingDesc(PassRefPtr<UPnPDevice> device)
{
    PendingDesc::iterator it = m_pendingDesc.find(device->udn());
    Vector<NetworkServiceDescriptionGupnp*> descriptions;
    String config = String(device->config());

    if (it == m_pendingDesc.end())
        return;

    descriptions = it->value;
    
    for (size_t i = 0; i < descriptions.size(); ++i) {
        NetworkServiceDescriptionGupnp* desc = descriptions[i];

        desc->update(desc->url(), config);
        
        addServiceDescription(desc);
    }
}

} // namespace WebCore

#endif // ENABLE(DISCOVERY)
