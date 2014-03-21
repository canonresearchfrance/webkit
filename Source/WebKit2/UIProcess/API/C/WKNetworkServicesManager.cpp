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
#include "WKNetworkServicesManager.h"

#include "WKAPICast.h"
#include "WebNetworkServicesManagerProxy.h"

using namespace WebKit;

WKTypeID WKNetworkServicesManagerGetTypeID()
{
#if ENABLE(DISCOVERY)
    return toAPI(WebNetworkServicesManagerProxy::APIType);
#else
    return 0;
#endif
}

void WKNetworkServicesManagerSetProvider(WKNetworkServicesManagerRef manager, const WKNetworkServicesProviderBase* provider)
{
#if ENABLE(DISCOVERY)
    toImpl(manager)->initializeProvider(provider);
#else
    UNUSED_PARAM(manager);
    UNUSED_PARAM(provider);
#endif
}

void WKNetworkServicesManagerProviderNotifyDiscoveryFinished(WKNetworkServicesManagerRef manager)
{
#if ENABLE(DISCOVERY)
    toImpl(manager)->didReceiveNotifyDiscoveryFinished();
#else
    UNUSED_PARAM(manager);
#endif
}

void WKNetworkServicesManagerProviderNotifyNetworkServiceChanged(WKNetworkServicesManagerRef manager, WKNetworkServiceRef service)
{
#if ENABLE(DISCOVERY)
    toImpl(manager)->didReceiveNetworkServiceChanged(toImpl(service));
#else
    UNUSED_PARAM(manager);
    UNUSED_PARAM(service);
#endif
}

void WKNetworkServicesManagerProviderNotifyNetworkServiceEvent(WKNetworkServicesManagerRef manager, WKNetworkServiceRef service, WKStringRef event)
{
#if ENABLE(DISCOVERY)
    toImpl(manager)->didReceiveNetworkServiceEvent(toImpl(service), toWTFString(event));
#else
    UNUSED_PARAM(manager);
    UNUSED_PARAM(service);
#endif
}

void WKNetworkServicesManagerProviderDispatchExistingNetworkService(WKNetworkServicesManagerRef manager, WKNetworkServiceRef service)
{
#if ENABLE(DISCOVERY)
    toImpl(manager)->didReceiveDispatchExistingNetworkService(toImpl(service));
#else
    UNUSED_PARAM(manager);
    UNUSED_PARAM(service);
#endif
}
