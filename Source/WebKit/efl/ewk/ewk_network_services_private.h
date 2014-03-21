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

#ifndef ewk_network_services_private_h
#define ewk_network_services_private_h
 
#if ENABLE(DISCOVERY)

#include "ewk_network_service.h"
#include "ewk_network_services.h"

// forward declarations
namespace WebCore {
class NetworkService;
class NetworkServices;
class NetworkServicesRequest;
}

Ewk_NetworkService* ewk_network_service_new(PassRefPtr<WebCore::NetworkService> core, unsigned long index);
void ewk_network_service_free(Ewk_NetworkService* service);
unsigned long ewk_network_service_index_get(Ewk_NetworkService* service);
Ewk_NetworkServices* ewk_network_services_new(PassRefPtr<WebCore::NetworkServicesRequest> core);
Ewk_NetworkServices* ewk_network_services_get(PassRefPtr<WebCore::NetworkServicesRequest> core);

namespace EWKPrivate {
WebCore::NetworkService* coreNetworkService(const Ewk_NetworkService* service);
}

#endif

#endif // ewk_network_services_private_h
