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
#include "webkitnetworkservice.h"

#if ENABLE(DISCOVERY)
#include "NetworkService.h"
#endif
#include "webkitnetworkservicesprivate.h"
#include "webkitglobalsprivate.h"
#include <glib/gi18n-lib.h>
#include <wtf/RefPtr.h>
#include <wtf/text/CString.h>

/**
 * SECTION:webkitnetworkservice
 * @short_description: Represent a view of a single local network service
 * 
 * #WebKitNetworkService objects are given to the application through the
 * #WebKitNetworkServices API. 
 */
G_DEFINE_TYPE(WebKitNetworkService, webkit_network_service, G_TYPE_OBJECT)

struct _WebKitNetworkServicePrivate {
#if ENABLE(DISCOVERY)
    RefPtr<NetworkService> service;
#endif
    gchar* id;
    gchar* name;
    gchar* type;
    gchar* url;
    gchar* config;
};

enum {
    PROP_0,
    PROP_ID,
    PROP_NAME,
    PROP_TYPE,
    PROP_URL,
    PROP_CONFIG,
    PROP_ONLINE,
    PROP_CORS_ENABLE
};

static void webkit_network_service_init(WebKitNetworkService* service)
{
    service->priv = G_TYPE_INSTANCE_GET_PRIVATE(service, WEBKIT_TYPE_NETWORK_SERVICE, WebKitNetworkServicePrivate);
    new (service->priv) WebKitNetworkServicePrivate();
}

static void webkit_network_service_finalize(GObject* object)
{
    WebKitNetworkService* service = WEBKIT_NETWORK_SERVICE(object);
    WebKitNetworkServicePrivate* priv = service->priv;

    g_free(priv->id);
    g_free(priv->name);
    g_free(priv->type);
    g_free(priv->url);
    g_free(priv->config);

    G_OBJECT_CLASS(webkit_network_service_parent_class)->finalize(object);
}

static void webkit_network_service_get_property(GObject* object, guint propId, GValue* value, GParamSpec* paramSpec)
{
    WebKitNetworkService* service = WEBKIT_NETWORK_SERVICE(object);
    switch (propId) {
    case PROP_ID:
        g_value_set_string(value, webkit_network_service_get_id(service));
        break;
    case PROP_NAME:
        g_value_set_string(value, webkit_network_service_get_name(service));
        break;
    case PROP_TYPE:
        g_value_set_string(value, webkit_network_service_get_servicetype(service));
        break;
    case PROP_URL:
        g_value_set_string(value, webkit_network_service_get_url(service));
        break;
    case PROP_CONFIG:
        g_value_set_string(value, webkit_network_service_get_config(service));
        break;
    case PROP_ONLINE:
        g_value_set_boolean(value, webkit_network_service_is_online(service));
        break;
    case PROP_CORS_ENABLE:
        g_value_set_boolean(value, webkit_network_service_is_cors_enable(service));
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, propId, paramSpec);
        break;
    }
}

static void webkit_network_service_class_init(WebKitNetworkServiceClass* serviceClass)
{
    GObjectClass* objectClass = G_OBJECT_CLASS(serviceClass);
    objectClass->finalize = webkit_network_service_finalize;
    objectClass->get_property = webkit_network_service_get_property;
    g_type_class_add_private(serviceClass, sizeof(WebKitNetworkServicePrivate));

    /**
     * WebKitNetworkService:id:
     *
     * The unique identifier for the given service. See
     * webkit_network_service_get_id() for more
     * details.
     *
     * Since: 2.4
     */
    g_object_class_install_property(objectClass,
                                    PROP_ID,
                                    g_param_spec_string("id",
                                                      _("Unique service identifier"),
                                                      _("The unique identifier associated to the service"),
                                                      "",
                                                      WEBKIT_PARAM_READABLE));

    /**
     * WebKitNetworkService:name:
     *
     * The name of the given service. See
     * webkit_network_service_get_name() for more
     * details.
     *
     * Since: 2.4
     */
    g_object_class_install_property(objectClass,
                                    PROP_NAME,
                                    g_param_spec_string("name",
                                                      _("Name of the service"),
                                                      _("The name of the service"),
                                                      "",
                                                      WEBKIT_PARAM_READABLE));

    /**
     * WebKitNetworkService:type:
     *
     * The type for the given service. See
     * webkit_network_service_get_servicetype() for more
     * details.
     *
     * Since: 2.4
     */
    g_object_class_install_property(objectClass,
                                    PROP_TYPE,
                                    g_param_spec_string("type",
                                                      _("Type of the service"),
                                                      _("The type of the service"),
                                                      "",
                                                      WEBKIT_PARAM_READABLE));

    /**
     * WebKitNetworkService:url:
     *
     * The absolute control url for the given service. See
     * webkit_network_service_get_url() for more
     * details.
     *
     * Since: 2.4
     */
    g_object_class_install_property(objectClass,
                                    PROP_URL,
                                    g_param_spec_string("url",
                                                      _("Absolute control url of the service"),
                                                      _("The absolute control url of the service"),
                                                      "",
                                                      WEBKIT_PARAM_READABLE));

    /**
     * WebKitNetworkService:config:
     *
     * The configuration for the given service. See
     * webkit_network_service_get_config() for more
     * details.
     *
     * Since: 2.4
     */
    g_object_class_install_property(objectClass,
                                    PROP_CONFIG,
                                    g_param_spec_string("config",
                                                      _("Configuration of the service"),
                                                      _("The configuration information associated with the service depending on the requested service type"),
                                                      "",
                                                      WEBKIT_PARAM_READABLE));

    /**
     * WebKitNetworkService:online:
     *
     * The current status of the given network service. See
     * webkit_network_service_is_online() for more
     * details.
     *
     * Since: 2.4
     */
    g_object_class_install_property(objectClass,
                                    PROP_ID,
                                    g_param_spec_boolean("online",
                                                       _("Status of the service"),
                                                       _("The current status of the service"),
                                                       TRUE,
                                                       WEBKIT_PARAM_READABLE));
    
    /**
     * WebKitNetworkService:corsEnable:
     *
     * The CORS ability of the given network service. See
     * webkit_network_service_is_cors_enable() for more
     * details.
     *
     * Since: 2.4
     */
    g_object_class_install_property(objectClass,
                                    PROP_ID,
                                    g_param_spec_boolean("enable-cors",
                                                       _("CORS ability of the service"),
                                                       _("The CORS ability of the service"),
                                                       FALSE,
                                                       WEBKIT_PARAM_READABLE));
}

#if ENABLE(DISCOVERY)
WebKitNetworkService* webkit_network_service_new(PassRefPtr<NetworkService> coreService)
{
    WebKitNetworkService* service = WEBKIT_NETWORK_SERVICE(g_object_new(WEBKIT_TYPE_NETWORK_SERVICE, NULL));
    service->priv->service = coreService;
    return service;
}
#endif

/**
 * webkit_network_service_get_id:
 * @service: a #WebKitNetworkService
 *
 * Get the unique identifier of the local network service,
 * in the format specified in the W3C Network Service Discovery specification. 
 * Its contents depend on the service type (UPnP, Zeroconf or DIAL).
 * This function should normally be called before presenting
 * the service list UI to the user, to decide whether to allow the
 * service or not.
 *
 * Returns: the unique identifier of the service.
 *
 * Since: 2.4
 */
const gchar* webkit_network_service_get_id(WebKitNetworkService* service)
{
    g_return_val_if_fail(WEBKIT_IS_NETWORK_SERVICE(service), NULL);

#if ENABLE(DISCOVERY)
    WebKitNetworkServicePrivate* priv = service->priv;
    WTF::String id = priv->service->id();

    g_free(priv->id);
    priv->id = g_strdup(id.utf8().data());
    return priv->id;
#else
    return NULL;
#endif
}

/**
 * webkit_network_service_get_name:
 * @service: a #WebKitNetworkService
 *
 * Get the name which represents a human-readable title for the service.
 *
 * Returns: the name of the service.
 *
 * Since: 2.4
 */
const gchar* webkit_network_service_get_name(WebKitNetworkService* service)
{
    g_return_val_if_fail(WEBKIT_IS_NETWORK_SERVICE(service), NULL);

#if ENABLE(DISCOVERY)
    WebKitNetworkServicePrivate* priv = service->priv;
    WTF::String name = priv->service->name();

    g_free(priv->name);
    priv->name = g_strdup(name.utf8().data());
    return priv->name;
#else
    return NULL;
#endif
}

/**
 * webkit_network_service_get_servicetype:
 * @service: a #WebKitNetworkService
 *
 * Get the type of the local network service,
 * in the format specified in the W3C Network Service Discovery specification. 
 * Its contents depend on the service type (start with "upnp:", "zeroconf:" or "dial:").
 *
 * Returns: the type of the service.
 *
 * Since: 2.4
 */
const gchar* webkit_network_service_get_servicetype(WebKitNetworkService* service)
{
    g_return_val_if_fail(WEBKIT_IS_NETWORK_SERVICE(service), NULL);

#if ENABLE(DISCOVERY)
    WebKitNetworkServicePrivate* priv = service->priv;
    WTF::String type = priv->service->type();

    g_free(priv->type);
    priv->type = g_strdup(type.utf8().data());
    return priv->type;
#else
    return NULL;
#endif
}

/**
 * webkit_network_service_get_url:
 * @service: a #WebKitNetworkService
 *
 * Get the absolute control url pointing to the root HTTP endpoint for the service.
 *
 * Returns: the absolute control url of the service.
 *
 * Since: 2.4
 */
const gchar* webkit_network_service_get_url(WebKitNetworkService* service)
{
    g_return_val_if_fail(WEBKIT_IS_NETWORK_SERVICE(service), NULL);

#if ENABLE(DISCOVERY)
    WebKitNetworkServicePrivate* priv = service->priv;
    WTF::String url = priv->service->url();

    g_free(priv->url);
    priv->url = g_strdup(url.utf8().data());
    return priv->url;
#else
    return NULL;
#endif
}

/**
 * webkit_network_service_get_config:
 * @service: a #WebKitNetworkService
 *
 * Get the raw configuration information extracted from the given network service.
 * Its contents depend on the service type. For UPnP services, the contents of the 
 * first occurrence of the <device> element in the device descriptor file. For Zeroconf,
 * the contents of the first DNS-SD TXT record associated with the service mDNS response.
 *
 * Returns: the config of the service.
 *
 * Since: 2.4
 */
const gchar* webkit_network_service_get_config(WebKitNetworkService* service)
{
    g_return_val_if_fail(WEBKIT_IS_NETWORK_SERVICE(service), NULL);

#if ENABLE(DISCOVERY)
    WebKitNetworkServicePrivate* priv = service->priv;
    WTF::String config = priv->service->config();

    g_free(priv->config);
    priv->config = g_strdup(config.utf8().data());
    return priv->config;
#else
    return NULL;
#endif
}

/**
 * webkit_network_service_is_online:
 * @service: a #WebKitNetworkService
 *
 * Get the online status of the local network service.
 *
 * Returns: the online status of the service.
 *
 * Since: 2.4
 */
gboolean webkit_network_service_is_online(WebKitNetworkService* service)
{
    g_return_val_if_fail(WEBKIT_IS_NETWORK_SERVICE(service), FALSE);

#if ENABLE(DISCOVERY)
    WebKitNetworkServicePrivate* priv = service->priv;
    return (priv->service->online() == true) ? TRUE : FALSE;
#else
    return FALSE;
#endif
}

/**
 * webkit_network_service_is_cors_enable:
 * @service: a #WebKitNetworkService
 *
 * Get the CORS ability of the local network service.
 *
 * Returns: the CORS ability of the service.
 *
 * Since: 2.4
 */
gboolean webkit_network_service_is_cors_enable(WebKitNetworkService* service)
{
    g_return_val_if_fail(WEBKIT_IS_NETWORK_SERVICE(service), FALSE);

#if ENABLE(DISCOVERY)
    WebKitNetworkServicePrivate* priv = service->priv;
    return (priv->service->isCorsEnable() == true) ? TRUE : FALSE;
#else
    return FALSE;
#endif
}

/**
 * webkit_network_service_set_allowed:
 * @service: a #WebKitNetworkService
 * @allowed: allowance for the service
 *
 * Given allowance for the local network service.
 *
 * Since: 2.4
 */
void webkit_network_service_set_allowed(WebKitNetworkService *service, gboolean allowed)
{
    g_return_if_fail(WEBKIT_IS_NETWORK_SERVICE(service));
    
#if ENABLE(DISCOVERY)
    WebKitNetworkServicePrivate* priv = service->priv;
    priv->service->setAllowed((allowed == TRUE) ? true : false);
#else
    UNUSED_PARAM(allowed);
#endif
}
