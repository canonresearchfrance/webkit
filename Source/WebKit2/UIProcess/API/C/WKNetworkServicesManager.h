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

#ifndef WKNetworkServicesManager_h
#define WKNetworkServicesManager_h

#include <WebKit2/WKBase.h>

#ifdef __cplusplus
extern "C" {
#endif

// Provider.
typedef void (*WKNetworkServicesProviderStartUpdatingCallback)(WKNetworkServicesManagerRef manager, const void* clientInfo);
typedef void (*WKNetworkServicesProviderStopUpdatingCallback)(WKNetworkServicesManagerRef manager, const void* clientInfo);
typedef void (*WKNetworkServicesProviderSendDiscoveredServicesCallback)(WKNetworkServicesManagerRef manager, const void* clientInfo);
typedef void (*WKNetworkServicesProviderSubscribeEventCallback)(WKNetworkServicesManagerRef manager, WKStringRef id, const void* clientInfo);

typedef struct WKNetworkServicesProviderBase {
    int                                                                 version;
    const void *                                                        clientInfo;
} WKNetworkServicesProviderBase;

typedef struct WKNetworkServicesProviderV0 {
    WKNetworkServicesProviderBase                                               base;

    WKNetworkServicesProviderStartUpdatingCallback                              startUpdating;
    WKNetworkServicesProviderStopUpdatingCallback                               stopUpdating;
    WKNetworkServicesProviderSendDiscoveredServicesCallback                     sendDiscoveredServices;
    WKNetworkServicesProviderSubscribeEventCallback                             subscribeEvent;
} WKNetworkServicesProviderV0;

WK_EXPORT WKTypeID WKNetworkServicesManagerGetTypeID();

WK_EXPORT void WKNetworkServicesManagerSetProvider(WKNetworkServicesManagerRef manager, const WKNetworkServicesProviderBase* provider);

WK_EXPORT void WKNetworkServicesManagerProviderNotifyDiscoveryFinished(WKNetworkServicesManagerRef manager);
WK_EXPORT void WKNetworkServicesManagerProviderNotifyNetworkServiceChanged(WKNetworkServicesManagerRef manager, WKNetworkServiceRef service);
WK_EXPORT void WKNetworkServicesManagerProviderNotifyNetworkServiceEvent(WKNetworkServicesManagerRef manager, WKNetworkServiceRef service, WKStringRef event);
WK_EXPORT void WKNetworkServicesManagerProviderDispatchExistingNetworkService(WKNetworkServicesManagerRef manager, WKNetworkServiceRef service);

#ifdef __cplusplus
}
#endif

#endif // WKNetworkServicesManager_h
