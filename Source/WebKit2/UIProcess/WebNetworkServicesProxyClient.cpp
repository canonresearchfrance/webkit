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
#include "WebNetworkServicesProxyClient.h"

#if ENABLE(DISCOVERY)

#include "WKAPICast.h"

using namespace WebCore;

namespace WebKit {

void WebNetworkServicesProxyClient::requestStarted(WebPageProxy* page, NetworkServicesRequestProxy* request)
{
    if (!m_client.requestStarted)
        return;

    m_client.requestStarted(toAPI(page), toAPI(request), m_client.base.clientInfo);
}

void WebNetworkServicesProxyClient::requestFinished(WebPageProxy* page)
{
    if (!m_client.requestFinished)
        return;

    m_client.requestFinished(toAPI(page), m_client.base.clientInfo);
}

void WebNetworkServicesProxyClient::requestUpdated(WebPageProxy* page, NetworkServicesRequestProxy* request)
{
    if (!m_client.requestUpdated)
        return;

    m_client.requestUpdated(toAPI(page), toAPI(request), m_client.base.clientInfo);
}

void WebNetworkServicesProxyClient::requestCanceled(WebPageProxy* page, NetworkServicesRequestProxy* request)
{
    if (!m_client.requestCanceled)
        return;

    m_client.requestCanceled(toAPI(page), toAPI(request), m_client.base.clientInfo);
}

} // namespace WebKit

#endif // ENABLE(DISCOVERY)
