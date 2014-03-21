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

#ifndef webkitnetworkservice_h
#define webkitnetworkservice_h

#include <gtk/gtk.h>
#include <webkit/webkitdefines.h>

G_BEGIN_DECLS

#define WEBKIT_TYPE_NETWORK_SERVICE             (webkit_network_service_get_type())
#define WEBKIT_NETWORK_SERVICE(obj)             (G_TYPE_CHECK_INSTANCE_CAST((obj), WEBKIT_TYPE_NETWORK_SERVICE, WebKitNetworkService))
#define WEBKIT_NETWORK_SERVICE_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST((klass),  WEBKIT_TYPE_NETWORK_SERVICE, WebKitNetworkServiceClass))
#define WEBKIT_IS_NETWORK_SERVICE(obj)          (G_TYPE_CHECK_INSTANCE_TYPE((obj), WEBKIT_TYPE_NETWORK_SERVICE))
#define WEBKIT_IS_NETWORK_SERVICE_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE((klass),  WEBKIT_TYPE_NETWORK_SERVICE))
#define WEBKIT_NETWORK_SERVICE_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS((obj),  WEBKIT_TYPE_NETWORK_SERVICE, WebKitNetworkServiceClass))

typedef struct _WebKitNetworkService        WebKitNetworkService;
typedef struct _WebKitNetworkServiceClass   WebKitNetworkServiceClass;
typedef struct _WebKitNetworkServicePrivate WebKitNetworkServicePrivate;

struct _WebKitNetworkService {
    GObject parent;

    /*< private >*/
    WebKitNetworkServicePrivate *priv;
};

struct _WebKitNetworkServiceClass {
    GObjectClass parent_class;
};

WEBKIT_API GType
webkit_network_service_get_type                  (void);

WEBKIT_API const gchar *
webkit_network_service_get_id           (WebKitNetworkService *service);

WEBKIT_API const gchar *
webkit_network_service_get_name         (WebKitNetworkService *service);

WEBKIT_API const gchar *
webkit_network_service_get_servicetype  (WebKitNetworkService *service);

WEBKIT_API const gchar *
webkit_network_service_get_url          (WebKitNetworkService *service);

WEBKIT_API const gchar *
webkit_network_service_get_config       (WebKitNetworkService *service);

WEBKIT_API gboolean
webkit_network_service_is_online        (WebKitNetworkService *service);

WEBKIT_API gboolean
webkit_network_service_is_cors_enable   (WebKitNetworkService *service);

WEBKIT_API void
webkit_network_service_set_allowed      (WebKitNetworkService *service, gboolean allowed);

G_END_DECLS

#endif // webkitnetworkservice_h
