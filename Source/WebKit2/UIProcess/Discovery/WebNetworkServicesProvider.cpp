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
#include "WebNetworkServicesProvider.h"

#if ENABLE(DISCOVERY)

#include "WKAPICast.h"
#include "WKRetainPtr.h"
#include "WebNetworkServicesManagerProxy.h"

using namespace WTF;
using namespace WebCore;

namespace WebKit {

WebNetworkServicesProvider::WebNetworkServicesProvider()
{
}

void WebNetworkServicesProvider::startUpdating(WebNetworkServicesManagerProxy* manager)
{
    if (!m_client.startUpdating)
        return;

    m_client.startUpdating(toAPI(manager), m_client.base.clientInfo);
}

void WebNetworkServicesProvider::stopUpdating(WebNetworkServicesManagerProxy* manager)
{
    if (!m_client.stopUpdating)
        return;

    m_client.stopUpdating(toAPI(manager), m_client.base.clientInfo);
}

void WebNetworkServicesProvider::sendDiscoveredServices(WebNetworkServicesManagerProxy* manager)
{
    if (!m_client.sendDiscoveredServices)
        return;

    m_client.sendDiscoveredServices(toAPI(manager), m_client.base.clientInfo);
}

void WebNetworkServicesProvider::subscribeEvent(WebNetworkServicesManagerProxy* manager, const String& event)
{
    if (!m_client.subscribeEvent)
        return;

    m_client.subscribeEvent(toAPI(manager), adoptWK(toCopiedAPI(event)).get(), m_client.base.clientInfo);
}

} // namespace WebKit

#endif // ENABLE(DISCOVERY)
