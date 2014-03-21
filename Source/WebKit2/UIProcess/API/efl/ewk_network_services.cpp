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

#include "EwkView.h"
#include "ewk_network_services_private.h"
#include <wtf/text/CString.h>

using namespace WebKit;

EwkNetworkServices::EwkNetworkServices(EwkView* view, WKNetworkServicesRequestRef request)
    : m_view(view)
    , m_request(request)
{
}

void EwkNetworkServices::addService(unsigned long index, PassRefPtr<EwkNetworkService> ewkServiceRef)
{
    m_services.add(index, ewkServiceRef);
}

PassRefPtr<EwkNetworkService> EwkNetworkServices::getService(unsigned long index)
{
    return m_services.get(index);
}
#endif // ENABLE(DISCOVERY)

unsigned long ewk_network_services_length_get(Ewk_NetworkServices* services)
{
#if ENABLE(DISCOVERY)
    EWK_OBJ_GET_IMPL_OR_RETURN(EwkNetworkServices, services, impl, 0);

    return WKNetworkServicesRequestGetLength(impl->wkNetworkServices());
#else
    UNUSED_PARAM(services);
    return 0;
#endif
}

unsigned long ewk_network_services_services_available_get(Ewk_NetworkServices* services)
{
#if ENABLE(DISCOVERY)
    EWK_OBJ_GET_IMPL_OR_RETURN(EwkNetworkServices, services, impl, 0);

    return WKNetworkServicesRequestGetAvailable(impl->wkNetworkServices());
#else
    UNUSED_PARAM(services);
    return 0;
#endif
}

Ewk_NetworkService* ewk_network_services_item_get(Ewk_NetworkServices* services, unsigned long index)
{
#if ENABLE(DISCOVERY)
    EWK_OBJ_GET_IMPL_OR_RETURN(EwkNetworkServices, services, impl, 0);

    RefPtr<EwkNetworkService> srvRef = impl->getService(index);

    if (!srvRef) {
        WKNetworkServiceRef wkService = WKNetworkServicesRequestGetItem(impl->wkNetworkServices(), index);
        
        srvRef = EwkNetworkService::create(impl, wkService);        
        impl->addService(index, srvRef);
    }

    return srvRef.get();
#else
    UNUSED_PARAM(services);
    UNUSED_PARAM(index);
    return 0;
#endif
}

const char* ewk_network_services_origin_get(Ewk_NetworkServices* services)
{
#if ENABLE(DISCOVERY)
    EWK_OBJ_GET_IMPL_OR_RETURN(EwkNetworkServices, services, impl, 0);

    WKRetainPtr<WKStringRef> origin = WKNetworkServicesRequestGetOrigin(impl->wkNetworkServices());

    return eina_stringshare_add(toWTFString(origin.get()).utf8().data());
#else
    UNUSED_PARAM(services);
    return 0;
#endif
}

void ewk_network_services_allowed_notify(Ewk_NetworkServices* services)
{
#if ENABLE(DISCOVERY)
    EWK_OBJ_GET_IMPL_OR_RETURN(EwkNetworkServices, services, impl);

    WKNetworkServicesRequestNotifyAllowed(impl->wkNetworkServices());
#else
    UNUSED_PARAM(services);
#endif
}

void ewk_network_services_denied_notify(Ewk_NetworkServices* services)
{
#if ENABLE(DISCOVERY)
    EWK_OBJ_GET_IMPL_OR_RETURN(EwkNetworkServices, services, impl);

    WKNetworkServicesRequestNotifyDenied(impl->wkNetworkServices());
#else
    UNUSED_PARAM(services);
#endif
}

