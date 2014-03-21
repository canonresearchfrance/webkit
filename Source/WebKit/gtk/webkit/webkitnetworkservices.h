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

#ifndef webkitnetworkservices_h
#define webkitnetworkservices_h

#include <gtk/gtk.h>
#include <webkit/webkitdefines.h>
#include "webkitnetworkservice.h"

G_BEGIN_DECLS

#define WEBKIT_TYPE_NETWORK_SERVICES             (webkit_network_services_get_type())
#define WEBKIT_NETWORK_SERVICES(obj)             (G_TYPE_CHECK_INSTANCE_CAST((obj), WEBKIT_TYPE_NETWORK_SERVICES, WebKitNetworkServices))
#define WEBKIT_NETWORK_SERVICES_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST((klass),  WEBKIT_TYPE_NETWORK_SERVICES, WebKitNetworkServicesClass))
#define WEBKIT_IS_NETWORK_SERVICES(obj)          (G_TYPE_CHECK_INSTANCE_TYPE((obj), WEBKIT_TYPE_NETWORK_SERVICES))
#define WEBKIT_IS_NETWORK_SERVICES_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE((klass),  WEBKIT_TYPE_NETWORK_SERVICES))
#define WEBKIT_NETWORK_SERVICES_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS((obj),  WEBKIT_TYPE_NETWORK_SERVICES, WebKitNetworkServicesClass))

typedef struct _WebKitNetworkServices        WebKitNetworkServices;
typedef struct _WebKitNetworkServicesClass   WebKitNetworkServicesClass;
typedef struct _WebKitNetworkServicesPrivate WebKitNetworkServicesPrivate;

struct _WebKitNetworkServices {
    GObject parent;

    /*< private >*/
    WebKitNetworkServicesPrivate *priv;
};

struct _WebKitNetworkServicesClass {
    GObjectClass parent_class;
};

WEBKIT_API GType
webkit_network_services_get_type                  (void);

WEBKIT_API const gchar *
webkit_network_services_get_origin                (WebKitNetworkServices *services);

WEBKIT_API unsigned long
webkit_network_services_get_length                (WebKitNetworkServices *services);

WEBKIT_API unsigned long
webkit_network_services_get_services_available    (WebKitNetworkServices *services);

WEBKIT_API GSList *
webkit_network_services_get_all_services          (WebKitNetworkServices *services);

WEBKIT_API void
webkit_network_services_notify_allowed            (WebKitNetworkServices *services);

WEBKIT_API void
webkit_network_services_notify_denied             (WebKitNetworkServices *services);

G_END_DECLS

#endif // webkitnetworkservices_h
