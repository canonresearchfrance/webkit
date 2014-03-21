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
#include "NetworkServiceDescriptionGupnp.h"

#if ENABLE(DISCOVERY)

namespace WebCore {

NetworkServiceDescriptionGupnp::NetworkServiceDescriptionGupnp(GUPnPServiceProxy* proxy, const char* id, const char* name, const char* type, const char* url, const char* config)
    : NetworkServiceDescription(id, name, type, url, config, 0)
    , m_proxy(proxy)
    , m_udn(String(gupnp_service_info_get_udn(GUPNP_SERVICE_INFO(proxy))))
{
}

void NetworkServiceDescriptionGupnp::subscribeEvent(GUPnPServiceProxyNotifyCallback callback, gpointer userData)
{
    gupnp_service_proxy_add_raw_notify(m_proxy, callback, userData);
    gupnp_service_proxy_set_subscribed(m_proxy, TRUE);
}

} // namespace WebCore

#endif // ENABLE(DISCOVERY)
