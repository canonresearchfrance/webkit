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

#include "ewk_network_service.h"

#if ENABLE(DISCOVERY)
#include "NetworkService.h"
#endif
#include "ewk_network_services_private.h"
#include "ewk_private.h"

/**
 * \struct  _Ewk_NetworkService
 * @brief   Contains the network service data.
 */
struct _Ewk_NetworkService {
#if ENABLE(DISCOVERY)
    RefPtr<WebCore::NetworkService> core; /**< WebCore object which is responsible for the service */
#endif
    const char* id;
    const char* name;
    const char* type;
    const char* url;
    const char* config;

    unsigned long index;
};

#define EWK_NETWORK_SERVICE_CORE_GET_OR_RETURN(service, core_, ...) \
    if (!(service)) {                                               \
        CRITICAL("service is NULL.");                               \
        return __VA_ARGS__;                                         \
    }                                                               \
    if (!(service)->core) {                                         \
        CRITICAL("service->core is NULL.");                         \
        return __VA_ARGS__;                                         \
    }                                                               \
    WebCore::NetworkService* core_ = (service)->core.get()

const char* ewk_network_service_id_get(const Ewk_NetworkService* service)
{
#if ENABLE(DISCOVERY)
    EWK_NETWORK_SERVICE_CORE_GET_OR_RETURN(service, core, 0);

    // hide the following optimization from outside
    Ewk_NetworkService* ewkService = const_cast<Ewk_NetworkService*>(service);
    eina_stringshare_replace(&ewkService->id,
                             core->id().utf8().data());
    return ewkService->id;
#else
    UNUSED_PARAM(service);
    return 0;
#endif
}

const char* ewk_network_service_name_get(const Ewk_NetworkService* service)
{
#if ENABLE(DISCOVERY)
    EWK_NETWORK_SERVICE_CORE_GET_OR_RETURN(service, core, 0);

    // hide the following optimization from outside
    Ewk_NetworkService* ewkService = const_cast<Ewk_NetworkService*>(service);
    eina_stringshare_replace(&ewkService->name,
                             core->name().utf8().data());
    return ewkService->name;
#else
    UNUSED_PARAM(service);
    return 0;
#endif
}

const char* ewk_network_service_type_get(const Ewk_NetworkService* service)
{
#if ENABLE(DISCOVERY)
    EWK_NETWORK_SERVICE_CORE_GET_OR_RETURN(service, core, 0);

    // hide the following optimization from outside
    Ewk_NetworkService* ewkService = const_cast<Ewk_NetworkService*>(service);
    eina_stringshare_replace(&ewkService->type,
                             core->type().utf8().data());
    return ewkService->type;
#else
    UNUSED_PARAM(service);
    return 0;
#endif
}

const char* ewk_network_service_url_get(const Ewk_NetworkService* service)
{
#if ENABLE(DISCOVERY)
    EWK_NETWORK_SERVICE_CORE_GET_OR_RETURN(service, core, 0);

    // hide the following optimization from outside
    Ewk_NetworkService* ewkService = const_cast<Ewk_NetworkService*>(service);
    eina_stringshare_replace(&ewkService->url,
                             core->url().utf8().data());
    return ewkService->url;
#else
    UNUSED_PARAM(service);
    return 0;
#endif
}

const char* ewk_network_service_config_get(const Ewk_NetworkService* service)
{
#if ENABLE(DISCOVERY)
    EWK_NETWORK_SERVICE_CORE_GET_OR_RETURN(service, core, 0);

    // hide the following optimization from outside
    Ewk_NetworkService* ewkService = const_cast<Ewk_NetworkService*>(service);
    eina_stringshare_replace(&ewkService->config,
                             core->config().utf8().data());
    return ewkService->config;
#else
    UNUSED_PARAM(service);
    return 0;
#endif
}

Eina_Bool ewk_network_service_is_online(const Ewk_NetworkService* service)
{
#if ENABLE(DISCOVERY)
    EWK_NETWORK_SERVICE_CORE_GET_OR_RETURN(service, core, 0);

    return core->online() ? EINA_TRUE : EINA_FALSE;
#else
    UNUSED_PARAM(service);
    return EINA_FALSE;
#endif
}

Eina_Bool ewk_network_service_is_cors_enable(const Ewk_NetworkService* service)
{
#if ENABLE(DISCOVERY)
    EWK_NETWORK_SERVICE_CORE_GET_OR_RETURN(service, core, 0);

    return core->isCorsEnable() ? EINA_TRUE : EINA_FALSE;
#else
    UNUSED_PARAM(service);
    return EINA_FALSE;
#endif
}

void ewk_network_service_allowed_set(Ewk_NetworkService* service, Eina_Bool allowed)
{
#if ENABLE(DISCOVERY)
    EWK_NETWORK_SERVICE_CORE_GET_OR_RETURN(service, core);

    core->setAllowed((allowed == EINA_TRUE) ? true : false);
#else
    UNUSED_PARAM(service);
    UNUSED_PARAM(allowed);
    return;
#endif
}

#if ENABLE(DISCOVERY)
/**
 * @internal
 *
 * Creates a new Ewk_NetworkService object.
 *
 * @param core WebCore::NetworkService instance to use internally.
 * @return a new allocated the Ewk_NetworkService object on success or @c 0 on failure
 */
Ewk_NetworkService* ewk_network_service_new(PassRefPtr<WebCore::NetworkService> core, unsigned long index)
{
    Ewk_NetworkService* service = new Ewk_NetworkService;
    memset(service, 0, sizeof(Ewk_NetworkService));
    service->core = core;
    service->index = index;
    eina_stringshare_replace(&service->id, service->core->id().utf8().data());
    eina_stringshare_replace(&service->name, service->core->name().utf8().data());
    eina_stringshare_replace(&service->type, service->core->type().utf8().data());
    eina_stringshare_replace(&service->url, service->core->url().utf8().data());
    eina_stringshare_replace(&service->config, service->core->config().utf8().data());

    return service;
}

/**
 * @internal
 *
 * Free given Ewk_NetworkService instance.
 *
 * @param service the object to free.
 */
void ewk_network_service_free(Ewk_NetworkService* service)
{
    EINA_SAFETY_ON_NULL_RETURN(service);

    eina_stringshare_del(service->id);
    eina_stringshare_del(service->name);
    eina_stringshare_del(service->type);
    eina_stringshare_del(service->url);
    eina_stringshare_del(service->config);
    delete service;
}

/**
 * @internal
 *
 * Query index for this network service.
 *
 * @param service network service item to query.
 * 
 * @return the index.
 */
unsigned long ewk_network_service_index_get(Ewk_NetworkService* service)
{
    EWK_NETWORK_SERVICE_CORE_GET_OR_RETURN(service, core, 0);
    UNUSED_PARAM(core);

    return service->index;
}

namespace EWKPrivate {

WebCore::NetworkService* coreNetworkService(const Ewk_NetworkService* service)
{
    EWK_NETWORK_SERVICE_CORE_GET_OR_RETURN(service, core, 0);
    return core;
}

} // namespace EWKPrivate

#endif
