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
#include "WKNetworkServicesRequest.h"

#include "NetworkServicesRequestProxy.h"
#include "WKAPICast.h"

using namespace WebKit;


unsigned long WKNetworkServicesRequestGetLength(WKNetworkServicesRequestRef request)
{
#if ENABLE(DISCOVERY)
    return toImpl(request)->length();
#else
    UNUSED_PARAM(request);
    return 0;
#endif
}

unsigned long WKNetworkServicesRequestGetAvailable(WKNetworkServicesRequestRef request)
{
#if ENABLE(DISCOVERY)
    return toImpl(request)->availableServices();
#else
    UNUSED_PARAM(request);
    return 0;
#endif
}

WKNetworkServiceRef WKNetworkServicesRequestGetItem(WKNetworkServicesRequestRef request, unsigned long index)
{
#if ENABLE(DISCOVERY)
    return toAPI((WebNetworkService *)(toImpl(request)->getServiceByIndex(index)));
#else
    UNUSED_PARAM(request);
    UNUSED_PARAM(index);
    return 0;
#endif
}

void WKNetworkServicesRequestSetServiceAlllowed(WKNetworkServicesRequestRef request, WKStringRef serviceID, bool allowed)
{
#if ENABLE(DISCOVERY)
    toImpl(request)->setServiceAllowed(toWTFString(serviceID), allowed);
#else
    UNUSED_PARAM(request);
    UNUSED_PARAM(serviceID);
    UNUSED_PARAM(allowed);
#endif
}

WKStringRef WKNetworkServicesRequestGetOrigin(WKNetworkServicesRequestRef request)
{
#if ENABLE(DISCOVERY)
    return toCopiedAPI(toImpl(request)->origin());
#else
    UNUSED_PARAM(request);
    return 0;
#endif
}

void WKNetworkServicesRequestNotifyAllowed(WKNetworkServicesRequestRef request)
{
#if ENABLE(DISCOVERY)
    toImpl(request)->allow();
#else
    UNUSED_PARAM(request);
#endif
}

void WKNetworkServicesRequestNotifyDenied(WKNetworkServicesRequestRef request)
{
#if ENABLE(DISCOVERY)
    toImpl(request)->deny();
#else
    UNUSED_PARAM(request);
#endif
}
