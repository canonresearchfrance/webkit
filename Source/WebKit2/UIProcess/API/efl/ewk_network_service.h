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
 * @file    ewk_network_service.h
 * @brief   Network Service API.
 *
 * Network Service APIs for services information getting.
 * This support attributes of network service regarding:
 * - getting service id
 * - getting service name
 * - getting service type
 * - getting service url
 * - getting service config
 * - getting service online status
 * - allowance of network service
 */

#ifndef ewk_network_service_h
#define ewk_network_service_h

#include <Eina.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \enum    _Ewk_NetworkService
 * @brief   Types of network service to support discovery.
 */
typedef struct EwkObject Ewk_NetworkService;

/**
 * Query id for this network service.
 *
 * @param service network service item to query.
 *
 * @return the id pointer, that may be @c NULL. This pointer is
 *         guaranteed to be eina_stringshare, so whenever possible
 *         save yourself some cpu cycles and use
 *         eina_stringshare_ref() instead of eina_stringshare_add() or
 *         strdup().
 */
EAPI const char* ewk_network_service_id_get(const Ewk_NetworkService* service);

/**
 * Query name for this network service.
 *
 * @param service network service item to query.
 *
 * @return the name pointer, that may be @c NULL. This pointer is
 *         guaranteed to be eina_stringshare, so whenever possible
 *         save yourself some cpu cycles and use
 *         eina_stringshare_ref() instead of eina_stringshare_add() or
 *         strdup().
 */
EAPI const char* ewk_network_service_name_get(const Ewk_NetworkService* service);

/**
 * Query type for this network service.
 *
 * @param service network service item to query.
 *
 * @return the type pointer, that may be @c NULL. This pointer is
 *         guaranteed to be eina_stringshare, so whenever possible
 *         save yourself some cpu cycles and use
 *         eina_stringshare_ref() instead of eina_stringshare_add() or
 *         strdup().
 */
EAPI const char* ewk_network_service_type_get(const Ewk_NetworkService* service);

/**
 * Query url for this network service.
 *
 * @param service network service item to query.
 *
 * @return the url pointer, that may be @c NULL. This pointer is
 *         guaranteed to be eina_stringshare, so whenever possible
 *         save yourself some cpu cycles and use
 *         eina_stringshare_ref() instead of eina_stringshare_add() or
 *         strdup().
 */
EAPI const char* ewk_network_service_url_get(const Ewk_NetworkService* service);

/**
 * Query config for this network service.
 *
 * @param service network service item to query.
 *
 * @return the config pointer, that may be @c NULL. This pointer is
 *         guaranteed to be eina_stringshare, so whenever possible
 *         save yourself some cpu cycles and use
 *         eina_stringshare_ref() instead of eina_stringshare_add() or
 *         strdup().
 */
EAPI const char* ewk_network_service_config_get(const Ewk_NetworkService* service);

/**
 * Query online status for this network service.
 *
 * @param service network service item to query.
 *
 * @return the online state.
 */
EAPI Eina_Bool ewk_network_service_is_online(const Ewk_NetworkService* service);

/**
 * Query CORS ability for this network service.
 *
 * @param service network service item to query.
 *
 * @return the CORS ability for the service.
 */
EAPI Eina_Bool ewk_network_service_is_cors_enable(const Ewk_NetworkService* service);

/**
 * Set allowed state for this network service.
 *
 * @param service network service item to set.
 * @param allowed state to set.
 */
EAPI void ewk_network_service_allowed_set(Ewk_NetworkService* service, Eina_Bool allowed);

#ifdef __cplusplus
}
#endif

#endif // ewk_network_service_h
