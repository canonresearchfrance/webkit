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

#include "WKNetworkService.h"
#include "WKString.h"
#include "ewk_network_services_private.h"
#include <wtf/text/CString.h>

using namespace WebKit;

EwkNetworkService::EwkNetworkService(EwkNetworkServices* ewkServices, WKNetworkServiceRef service)
    : m_networkServices(ewkServices)
    , m_service(service)
{
}

WKStringRef EwkNetworkService::id() const 
{ 
    return WKNetworkServiceGetId(m_service.get()); 
}

WKStringRef EwkNetworkService::name() const 
{ 
    return WKNetworkServiceGetName(m_service.get()); 
}

WKStringRef EwkNetworkService::type() const 
{ 
    return WKNetworkServiceGetType(m_service.get()); 
}

WKStringRef EwkNetworkService::url() const 
{ 
    return WKNetworkServiceGetUrl(m_service.get()); 
}

WKStringRef EwkNetworkService::config() const 
{ 
    return WKNetworkServiceGetConfig(m_service.get()); 
}

bool EwkNetworkService::online() const
{ 
    return WKNetworkServiceIsOnline(m_service.get()); 
}

bool EwkNetworkService::corsEnable() const
{ 
    return WKNetworkServiceIsCorsEnable(m_service.get()); 
}

void EwkNetworkService::setAllowed(bool allowed) 
{
    WKNetworkServicesRequestSetServiceAlllowed(m_networkServices->wkNetworkServices(), id(), allowed);
}
#endif // ENABLE(DISCOVERY)

const char* ewk_network_service_id_get(const Ewk_NetworkService* service)
{
#if ENABLE(DISCOVERY)
    EWK_OBJ_GET_IMPL_OR_RETURN(const EwkNetworkService, service, impl, 0);

    return eina_stringshare_add(toWTFString(impl->id()).utf8().data());
#else
    UNUSED_PARAM(service);
    return 0;
#endif
}

const char* ewk_network_service_name_get(const Ewk_NetworkService* service)
{
#if ENABLE(DISCOVERY)
    EWK_OBJ_GET_IMPL_OR_RETURN(const EwkNetworkService, service, impl, 0);

    return eina_stringshare_add(toWTFString(impl->name()).utf8().data());
#else
    UNUSED_PARAM(service);
    return 0;
#endif
}

const char* ewk_network_service_type_get(const Ewk_NetworkService* service)
{
#if ENABLE(DISCOVERY)
    EWK_OBJ_GET_IMPL_OR_RETURN(const EwkNetworkService, service, impl, 0);

    return eina_stringshare_add(toWTFString(impl->type()).utf8().data());
#else
    UNUSED_PARAM(service);
    return 0;
#endif
}

const char* ewk_network_service_url_get(const Ewk_NetworkService* service)
{
#if ENABLE(DISCOVERY)
    EWK_OBJ_GET_IMPL_OR_RETURN(const EwkNetworkService, service, impl, 0);

    return eina_stringshare_add(toWTFString(impl->url()).utf8().data());
#else
    UNUSED_PARAM(service);
    return 0;
#endif
}

const char* ewk_network_service_config_get(const Ewk_NetworkService* service)
{
#if ENABLE(DISCOVERY)
    EWK_OBJ_GET_IMPL_OR_RETURN(const EwkNetworkService, service, impl, 0);

    return eina_stringshare_add(toWTFString(impl->config()).utf8().data());
#else
    UNUSED_PARAM(service);
    return 0;
#endif
}

Eina_Bool ewk_network_service_is_online(const Ewk_NetworkService* service)
{
#if ENABLE(DISCOVERY)
    EWK_OBJ_GET_IMPL_OR_RETURN(const EwkNetworkService, service, impl, EINA_FALSE);

    return impl->online() ? EINA_TRUE : EINA_FALSE;
#else
    UNUSED_PARAM(service);
    return EINA_FALSE;
#endif
}

Eina_Bool ewk_network_service_is_cors_enable(const Ewk_NetworkService* service)
{
#if ENABLE(DISCOVERY)
    EWK_OBJ_GET_IMPL_OR_RETURN(const EwkNetworkService, service, impl, EINA_FALSE);

    return impl->corsEnable() ? EINA_TRUE : EINA_FALSE;
#else
    UNUSED_PARAM(service);
    return EINA_FALSE;
#endif
}

void ewk_network_service_allowed_set(Ewk_NetworkService* service, Eina_Bool allowed)
{
#if ENABLE(DISCOVERY)
    EWK_OBJ_GET_IMPL_OR_RETURN(EwkNetworkService, service, impl);

    impl->setAllowed((allowed == EINA_TRUE) ? true : false);
#else
    UNUSED_PARAM(service);
#endif
}

