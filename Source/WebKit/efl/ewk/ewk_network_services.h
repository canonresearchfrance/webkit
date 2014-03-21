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

/**
 * @file    ewk_network_services.h
 * @brief   Network Services API.
 *
 * There is one Ewk_NetworkServices object associated to getNetworkServices request.
 * This opaque object able to control the request lifecycle through the "notify" API.
 * A request contains information like requested types and callbacks associated to the request. 
 *
 * The WebView provide three smart callbacks:
 * - "networkservices,request,started"  : call for each getNetworkServices request. 
 *                                        The user shall register the request id.
 * - "networkservices,request,updated"  : call during the request permission phase on a LAN update
 *                                        The user may update the services list.
 * - "networkservices,request,finished" : call when a service discovery is finish. 
 *                                        The user shall provide permissions with the Ewk_NetworkService API.
 *
 * Network services APIs able to:
 * - release Ewk_NetworkServices previously allocated.
 * - getting length of the network services list
 * - getting the current number of available services
 * - getting network service from index in service list
 * - allowance of notification
 */

#ifndef ewk_network_services_h
#define ewk_network_services_h

#include <Eina.h>
#include "ewk_network_service.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Creates a type name for _Ewk_NetworkServices */
typedef struct _Ewk_NetworkServices Ewk_NetworkServices;

/**
 * Free given Ewk_NetworkServices instance.
 *
 * @param services the object to free.
 */
EAPI void ewk_network_services_free(Ewk_NetworkServices* networkServices);

/**
 * Retrieves the number of available services.
 *
 * @param services the network services list.
 *
 * @return the number of available services.
 */
EAPI unsigned long ewk_network_services_length_get(const Ewk_NetworkServices* services);

/**
 * Retrieves the number of available services.
 *
 * @param services the network services list.
 *
 * @return the number of available services.
 */
EAPI unsigned long ewk_network_services_services_available_get(const Ewk_NetworkServices* services);

/**
 * Query a service by index from this network services.
 *
 * The returned value should be freed by ewk_network_service_free().
 *
 * @param services network services item to query.
 * @param index service index query.
 *
 * @return a new allocated the Ewk_NetworkService object if found or @c 0.
 */
EAPI Ewk_NetworkService* ewk_network_services_item_get(Ewk_NetworkServices* services, unsigned long index);

/**
 * Retrieves the origin of the network services request.
 *
 * @param services the network services to query.
 *
 * @return the origin of the network services request.
 */
EAPI const char* ewk_network_services_origin_get(const Ewk_NetworkServices* services);

/**
 * Notify allowed services.
 * Only the first call has effect. This notification provide user permission 
 * and finish the request by calling appropriated callbacks.
 *
 * @param services network services item to notify.
 */
EAPI void ewk_network_services_allowed_notify(const Ewk_NetworkServices* services);

/**
 * Notify denied services.
 *
 * @param services network services item to notify.
 */
EAPI void ewk_network_services_denied_notify(const Ewk_NetworkServices* services);

#ifdef __cplusplus
}
#endif

#endif // ewk_network_services_h
