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
#include "NetworkServicePublisherGupnpMock.h"

#if ENABLE(DISCOVERY)

#include "Logging.h"
#include <libgupnp/gupnp.h>

namespace WebCore {

class ServiceInfoGupnpMock {
public:
    ServiceInfoGupnpMock();
    ~ServiceInfoGupnpMock();

    static GUPnPContext* m_gupnpContext;

    GUPnPRootDevice* m_rootDevice;
    GUPnPServiceInfo* m_service;
    gboolean status;
};

GUPnPContext* ServiceInfoGupnpMock::m_gupnpContext = 0;

static HashMap<const char*, ServiceInfoGupnpMock*> m_services;

ServiceInfoGupnpMock::ServiceInfoGupnpMock()
{
    if (!m_gupnpContext) {
        /* Create the GUPnP context with default host and port */
        m_gupnpContext = gupnp_context_new(0, 0, 0, 0);
    }
    
}

ServiceInfoGupnpMock::~ServiceInfoGupnpMock()
{
    g_object_unref(m_service);
    g_object_unref(m_rootDevice);
    g_object_unref(m_gupnpContext);
}

/* SwitchPower service callbacks */
static
void setTargetCallback(GUPnPService* service, GUPnPServiceAction* action, gpointer userData)
{
    gboolean target;
    ServiceInfoGupnpMock* si = static_cast<ServiceInfoGupnpMock*>(userData);
    
    /* Get the new target value */
    gupnp_service_action_get(action, "NewTargetValue", G_TYPE_BOOLEAN, &target, NULL);

    /* If the new target doesn't match the current status, change the status and
        emit a notification that the status has changed. */
    if (target != si->status) {
        si->status = target;
        gupnp_service_notify(service, "Status", G_TYPE_BOOLEAN, (si->status == TRUE) ? "1" : "0", NULL);

        // g_print ("The light is now %s.\n", si->status ? "on" : "off");
    }

    /* Return success to the client */
    gupnp_service_action_return(action);
}

static
void getTargetCallback(GUPnPService *service, GUPnPServiceAction *action, gpointer userData)
{
    ServiceInfoGupnpMock* si = static_cast<ServiceInfoGupnpMock*>(userData);

    UNUSED_PARAM(service);

    gupnp_service_action_set(action, "RetTargetValue", G_TYPE_BOOLEAN, si->status, NULL);
    gupnp_service_action_return(action);
}

static
void getStatusCallback(GUPnPService *service, GUPnPServiceAction *action, gpointer userData)
{
    ServiceInfoGupnpMock* si = static_cast<ServiceInfoGupnpMock*>(userData);

    UNUSED_PARAM(service);

    gupnp_service_action_set(action, "ResultStatus", G_TYPE_BOOLEAN, si->status, NULL);
    gupnp_service_action_return(action);
}

static
void queryTargetCallback(GUPnPService *service, char *variable, GValue *value, gpointer userData)
{
    ServiceInfoGupnpMock* si = static_cast<ServiceInfoGupnpMock*>(userData);

    UNUSED_PARAM(service);
    UNUSED_PARAM(variable);

    g_value_init(value, G_TYPE_BOOLEAN);
    g_value_set_boolean(value, si->status);
}

static
void queryStatusCallback(GUPnPService *service, char *variable, GValue *value, gpointer userData)
{
    ServiceInfoGupnpMock* si = static_cast<ServiceInfoGupnpMock*>(userData);

    UNUSED_PARAM(service);
    UNUSED_PARAM(variable);

    g_value_init(value, G_TYPE_BOOLEAN);
    g_value_set_boolean(value, si->status);
}

void NetworkServicePublisherGupnpMock::addSwitchPowerService()
{
    ServiceInfoGupnpMock* si = new ServiceInfoGupnpMock;

    if (!si)
        return;

#ifndef NETWORK_SERVICES_RESOURCES_DIR
#define NETWORK_SERVICES_RESOURCES_DIR DATA_DIR
#endif
    /* Create the root device object */
    si->m_rootDevice = gupnp_root_device_new(ServiceInfoGupnpMock::m_gupnpContext, "BinaryLight1.xml", NETWORK_SERVICES_RESOURCES_DIR);

    /* Activate the root device, so that it announces itself */ 
    gupnp_root_device_set_available(si->m_rootDevice, TRUE);

    /* Get the switch service from the root device */
    si->m_service = gupnp_device_info_get_service
        (GUPNP_DEVICE_INFO(si->m_rootDevice), "urn:schemas-upnp-org:service:SwitchPower:1");
    if (!si->m_service) {
        g_printerr("Cannot get SwitchPower1 service\n");
        return;
    }

    si->status = FALSE;

    g_signal_connect(si->m_service, "action-invoked::SetTarget", G_CALLBACK(setTargetCallback), si);
    g_signal_connect(si->m_service, "action-invoked::GetTarget", G_CALLBACK(getTargetCallback), si);
    g_signal_connect(si->m_service, "action-invoked::GetStatus", G_CALLBACK(getStatusCallback), si);
    g_signal_connect(si->m_service, "query-variable::Target", G_CALLBACK(queryTargetCallback), si);
    g_signal_connect(si->m_service, "query-variable::Status", G_CALLBACK(queryStatusCallback), si);

    m_services.add("SwitchPower1", si);
}

void NetworkServicePublisherGupnpMock::removeSwitchPowerService()
{
    ServiceInfoGupnpMock* si = m_services.get("SwitchPower1");

    if (si) {
        m_services.remove("SwitchPower1");
        delete si;
    }
}

void NetworkServicePublisherGupnpMock::removeAllServices()
{
}

} // namespace WebCore

#endif // ENABLE(DISCOVERY)
