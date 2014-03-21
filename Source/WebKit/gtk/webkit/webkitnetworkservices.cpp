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
#include "webkitnetworkservices.h"


#if ENABLE(DISCOVERY)
#include "NetworkService.h"
#include "NetworkServices.h"
#include "NetworkServicesRequest.h"
#endif
#include "webkitnetworkservicesprivate.h"
#include "webkitglobalsprivate.h"
#include <glib/gi18n-lib.h>
#include <wtf/RefPtr.h>

/**
 * SECTION:webkitnetworkservices
 * @short_description: Represent a view of a network services request permission
 * 
 * #WebKitNetworkServices objects are given to the application when
 * networkservices-request-started signal is emmitted. The application uses it 
 * to tell the engine whether it wants to allow or deny local network access 
 * for a given frame.
 */
G_DEFINE_TYPE(WebKitNetworkServices, webkit_network_services, G_TYPE_OBJECT)

struct _WebKitNetworkServicesPrivate {
#if ENABLE(DISCOVERY)
    NetworkServices* core;
    RefPtr<NetworkServicesRequest> core_request;
#endif
    gchar* origin;
    GSList* list;
};

enum {
    PROP_0,
    PROP_ORIGIN
};

static GSList* networkServicesList = 0;

static void webkit_network_services_init(WebKitNetworkServices* services)
{
    services->priv = G_TYPE_INSTANCE_GET_PRIVATE(services, WEBKIT_TYPE_NETWORK_SERVICES, WebKitNetworkServicesPrivate);
    new (services->priv) WebKitNetworkServicesPrivate();
}

static void webkit_network_services_finalize(GObject* object)
{
    WebKitNetworkServices* services = WEBKIT_NETWORK_SERVICES(object);
    WebKitNetworkServicesPrivate* priv = services->priv;

    g_free(priv->origin);
    g_slist_free(priv->list);

    networkServicesList = g_slist_remove(networkServicesList, services);

    G_OBJECT_CLASS(webkit_network_services_parent_class)->finalize(object);
}

static void webkit_network_services_get_property(GObject* object, guint propId, GValue* value, GParamSpec* paramSpec)
{
    WebKitNetworkServices* services = WEBKIT_NETWORK_SERVICES(object);
    switch (propId) {
    case PROP_ORIGIN:
        g_value_set_string(value, webkit_network_services_get_origin(services));
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, propId, paramSpec);
        break;
    }
}

static void webkit_network_services_class_init(WebKitNetworkServicesClass* servicesClass)
{
    GObjectClass* objectClass = G_OBJECT_CLASS(servicesClass);
    objectClass->finalize = webkit_network_services_finalize;
    objectClass->get_property = webkit_network_services_get_property;
    g_type_class_add_private(servicesClass, sizeof(WebKitNetworkServicesPrivate));

    /**
     * WebKitNetworkServices:origin:
     *
     * The origin domain of the given request. See
     * webkit_network_services_get_origin() for more
     * details.
     *
     * Since: 2.4
     */
    g_object_class_install_property(objectClass,
                                    PROP_ORIGIN,
                                    g_param_spec_string("origin",
                                                      _("Request origin domain"),
                                                      _("The origin domain of the request"),
                                                      "",
                                                      WEBKIT_PARAM_READABLE));
}

#if ENABLE(DISCOVERY)
WebKitNetworkServices* webkit_network_services_new(PassRefPtr<WebCore::NetworkServicesRequest> core)
{
    WebKitNetworkServices* services = WEBKIT_NETWORK_SERVICES(g_object_new(WEBKIT_TYPE_NETWORK_SERVICES, NULL));
    services->priv->core = core->networkServices().get();
    services->priv->core_request = core;
    services->priv->list = 0;

    networkServicesList = g_slist_prepend(networkServicesList, services);

    return services;
}

static gint compareWebKitNetworkServices(WebKitNetworkServices* ns1, WebKitNetworkServices* ns2) 
{
    return (ns1->priv->core_request.get() == ns2->priv->core_request.get()) ? 0 : -1;
}

WebKitNetworkServices* webkit_network_services_get(PassRefPtr<WebCore::NetworkServicesRequest> core)
{
    WebKitNetworkServices services;
    WebKitNetworkServicesPrivate priv;
    GSList* found;

    priv.core_request = core;
    services.priv = &priv;

    found = g_slist_find_custom(networkServicesList, &services, (GCompareFunc)compareWebKitNetworkServices);

    return found ? (WebKitNetworkServices*)(found->data) : NULL;
}
#endif

/**
 * webkit_network_services_get_origin:
 * @services: a #WebKitNetworkServices
 *
 * Get the origin domain of the local network services request.
 *
 * Returns: the origin domain of the request.
 *
 * Since: 2.4
 */
const gchar* webkit_network_services_get_origin(WebKitNetworkServices* services)
{
    g_return_val_if_fail(WEBKIT_IS_NETWORK_SERVICES(services), NULL);

#if ENABLE(DISCOVERY)
    WebKitNetworkServicesPrivate* priv = services->priv;
    WTF::String origin = priv->core_request->client()->getOrigin();

    g_free(priv->origin);
    priv->origin = g_strdup(origin.utf8().data());    
    return priv->origin;
#else
    return NULL;
#endif
}

/**
 * webkit_network_services_get_length:
 * @services: a #WebKitNetworkServices
 *
 * Get the number of services for the local network services request.
 *
 * Returns: the number of services for the request.
 *
 * Since: 2.4
 */
unsigned long webkit_network_services_get_length(WebKitNetworkServices *services)
{
    g_return_val_if_fail(WEBKIT_IS_NETWORK_SERVICES(services), 0);

#if ENABLE(DISCOVERY)
    WebKitNetworkServicesPrivate* priv = services->priv;
    return priv->core->length();
#else
    return 0;
#endif
}

/**
 * webkit_network_services_get_services_available:
 * @services: a #WebKitNetworkServices
 *
 * Get the number of available services for the local network services request.
 *
 * Returns: the number of available services for the request.
 *
 * Since: 2.4
 */
unsigned long webkit_network_services_get_services_available(WebKitNetworkServices *services)
{
    g_return_val_if_fail(WEBKIT_IS_NETWORK_SERVICES(services), 0);

#if ENABLE(DISCOVERY)
    WebKitNetworkServicesPrivate* priv = services->priv;
    return priv->core->servicesAvailable();
#else
    return 0;
#endif
}

/**
 * webkit_network_services_get_all_services:
 * @services: a #WebKitNetworkServices
 *
 * Get all services for the local network services request.
 *
 * Return value: (element-type WebKitNetworkService) (transfer container): a #GSList of #WebKitNetworkService.
 *
 * Since: 2.4
 */
GSList* webkit_network_services_get_all_services(WebKitNetworkServices *services)
{
    g_return_val_if_fail(WEBKIT_IS_NETWORK_SERVICES(services), NULL);

#if ENABLE(DISCOVERY)
    WebKitNetworkServicesPrivate* priv = services->priv;
    WebKitNetworkService* service;
    unsigned int i;

    g_slist_free(priv->list);
    priv->list = 0;

    for (i = 0; i < priv->core->length(); i++) {
        service = webkit_network_service_new(priv->core->item(i));
        priv->list = g_slist_prepend(priv->list, service);
    }

    return priv->list;
#else
    return NULL;
#endif
}

/**
 * webkit_network_services_notify_allowed:
 * @services: a #WebKitNetworkServices
 *
 * Given allowance for the local network services request.
 *
 * Since: 2.4
 */
void webkit_network_services_notify_allowed(WebKitNetworkServices *services)
{
    g_return_if_fail(WEBKIT_IS_NETWORK_SERVICES(services));

#if ENABLE(DISCOVERY)
    WebKitNetworkServicesPrivate* priv = services->priv;
    priv->core_request->setAllowed();
#endif
}

/**
 * webkit_network_services_notify_denied:
 * @services: a #WebKitNetworkServices
 *
 * Decline allowance for the local network services request.
 *
 * Since: 2.4
 */
void webkit_network_services_notify_denied(WebKitNetworkServices *services)
{
    g_return_if_fail(WEBKIT_IS_NETWORK_SERVICES(services));

#if ENABLE(DISCOVERY)
    WebKitNetworkServicesPrivate* priv = services->priv;
    priv->core_request->setDenied();
#endif
}
