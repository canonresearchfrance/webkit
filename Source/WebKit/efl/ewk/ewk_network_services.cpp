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

#include "ewk_network_services.h"

#if ENABLE(DISCOVERY)
#include "NetworkService.h"
#include "NetworkServices.h"
#include "NetworkServicesRequest.h"
#endif
#include "ewk_network_services_private.h"
#include "ewk_private.h"
#include <Eina.h>

/**
 * \struct  _Ewk_NetworkServices
 * @brief   Contains the network services data.
 */
struct _Ewk_NetworkServices {
#if ENABLE(DISCOVERY)
    WebCore::NetworkServices* core; /**< WebCore object which is responsible for the network services */
    RefPtr<WebCore::NetworkServicesRequest> core_request; /**< WebCore object which is responsible for the network services request*/
#endif
    const char* origin;
    Eina_List* list;
};

#define EWK_NETWORK_SERVICES_CORE_GET_OR_RETURN(services, core_, ...)    \
    if (!(services)) {                                                   \
        CRITICAL("services is NULL.");                                   \
        return __VA_ARGS__;                                              \
    }                                                                    \
    if (!(services)->core) {                                             \
        CRITICAL("services->core is NULL.");                             \
        return __VA_ARGS__;                                              \
    }                                                                    \
    WebCore::NetworkServices* core_ = (services)->core


#if ENABLE(DISCOVERY)
static Eina_List* s_ewkServices = 0;
#endif

/**
 * Free given Ewk_NetworkServices instance.
 *
 * @param services the object to free.
 */
void ewk_network_services_free(Ewk_NetworkServices* services)
{
#if ENABLE(DISCOVERY)
    void* service;
    EINA_SAFETY_ON_NULL_RETURN(services);

    s_ewkServices = eina_list_remove(s_ewkServices, services);

    eina_stringshare_del(services->origin);

    EINA_LIST_FREE(services->list, service)
        ewk_network_service_free(static_cast<Ewk_NetworkService*>(service));

    delete services;
#else
    UNUSED_PARAM(services);
#endif
}

unsigned long ewk_network_services_length_get(const Ewk_NetworkServices* services)
{
#if ENABLE(DISCOVERY)
    EWK_NETWORK_SERVICES_CORE_GET_OR_RETURN(services, core, 0);

    return core->length();
#else
    EINA_SAFETY_ON_NULL_RETURN_VAL(services, 0);
    return 0;
#endif
}

unsigned long ewk_network_services_services_available_get(const Ewk_NetworkServices* services)
{
#if ENABLE(DISCOVERY)
    EWK_NETWORK_SERVICES_CORE_GET_OR_RETURN(services, core, 0);

    return core->servicesAvailable();
#else
    EINA_SAFETY_ON_NULL_RETURN_VAL(services, 0);
    return 0;
#endif
}

Ewk_NetworkService* ewk_network_services_item_get(Ewk_NetworkServices* services, unsigned long index)
{
#if ENABLE(DISCOVERY)
    Eina_List* l;
    Eina_List* l_next;
    void *service;
    Ewk_NetworkService* ewkService;
    RefPtr<WebCore::NetworkService> coreService;

    EWK_NETWORK_SERVICES_CORE_GET_OR_RETURN(services, core, 0);

    EINA_LIST_FOREACH_SAFE(services->list, l, l_next, service)
    {
        ewkService = static_cast<Ewk_NetworkService*>(service);
        if (index == ewk_network_service_index_get(ewkService))
            return ewkService;
    }
    
    coreService = core->item(index);
    ewkService = ewk_network_service_new(coreService, index);
    services->list = eina_list_append(services->list, ewkService);

    return ewkService;
#else
	  EINA_SAFETY_ON_NULL_RETURN_VAL(services, 0);
	  UNUSED_PARAM(index);
    return 0;
#endif
}

EAPI const char* ewk_network_services_origin_get(const Ewk_NetworkServices* services)
{
#if ENABLE(DISCOVERY)
    if (!(services)) {
        CRITICAL("services is NULL.");
        return 0;
    }
    if (!(services)->core_request) {
        CRITICAL("services->core is NULL.");
        return 0;
    }
    
    Ewk_NetworkServices* ewkServices = const_cast<Ewk_NetworkServices*>(services);
    eina_stringshare_replace(&ewkServices->origin,
                            services->core_request->client()->getOrigin().utf8().data());
    return ewkServices->origin;
#else
    EINA_SAFETY_ON_NULL_RETURN_VAL(services, 0);
    return 0;
#endif
}

EAPI void ewk_network_services_allowed_notify(const Ewk_NetworkServices* services)
{
#if ENABLE(DISCOVERY)
    if (!(services)) {
        CRITICAL("services is NULL.");
        return;
    }
    if (!(services)->core_request) {
        CRITICAL("services->core is NULL.");
        return;
    }

    services->core_request->setAllowed();
#else
    EINA_SAFETY_ON_NULL_RETURN(services);
#endif
}

EAPI void ewk_network_services_denied_notify(const Ewk_NetworkServices* services)
{
#if ENABLE(DISCOVERY)
    if (!(services)) {
        CRITICAL("services is NULL.");
        return;
    }
    if (!(services)->core_request) {
        CRITICAL("services->core is NULL.");
        return;
    }

    services->core_request->setDenied();
#else
    EINA_SAFETY_ON_NULL_RETURN(services);
#endif
}

#if ENABLE(DISCOVERY)
/**
 * @internal
 *
 * Creates a new Ewk_NetworkServices object.
 *
 * @param core WebCore::NetworkServices instance to use internally.
 * @return a new allocated the Ewk_NetworkServices object on success or @c 0 on failure
 */
Ewk_NetworkServices* ewk_network_services_new(PassRefPtr<WebCore::NetworkServicesRequest> core)
{
    EINA_SAFETY_ON_NULL_RETURN_VAL(core, 0);

    Ewk_NetworkServices* services = new Ewk_NetworkServices;
    memset(services, 0, sizeof(Ewk_NetworkServices));
    services->core = core->networkServices().get();
    services->core_request = core;
    services->list = 0;

    s_ewkServices = eina_list_append(s_ewkServices, services);

    return services;
}

/**
 * @internal
 *
 * Retrieve an Ewk_NetworkServices object from the list.
 *
 * @param core WebCore::NetworkServices instance to retrieve.
 * @return the Ewk_NetworkServices object on success or @c 0 on failure
 */
Ewk_NetworkServices* ewk_network_services_get(PassRefPtr<WebCore::NetworkServicesRequest> core)
{
    Eina_List* l;
    Eina_List* l_next;
    void* services;
    Ewk_NetworkServices* ewkServices;

    EINA_SAFETY_ON_NULL_RETURN_VAL(core, 0);

    //
    EINA_LIST_FOREACH_SAFE(s_ewkServices, l, l_next, services)
    {
        ewkServices = static_cast<Ewk_NetworkServices*>(services);
        if (core == ewkServices->core_request)
            return ewkServices;
    }

    return 0;
}
#endif
