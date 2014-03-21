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

#ifndef WKPageNetworkServicesClient_h
#define WKPageNetworkServicesClient_h

#include <WebKit2/WKBase.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*WKPageNetworkServicesRequestStartedCallback)(WKPageRef page, WKNetworkServicesRequestRef request, const void* clientInfo);
typedef void (*WKPageNetworkServicesRequestFinishedCallback)(WKPageRef page, const void* clientInfo);
typedef void (*WKPageNetworkServicesRequestUpdatedCallback)(WKPageRef page, WKNetworkServicesRequestRef request, const void* clientInfo);
typedef void (*WKPageNetworkServicesRequestCanceledCallback)(WKPageRef page, WKNetworkServicesRequestRef request, const void* clientInfo);

typedef struct WKPageNetworkServicesClientBase {
    int                                                                 version;
    const void *                                                        clientInfo;
} WKPageNetworkServicesClientBase;

typedef struct WKPageNetworkServicesClientV0 {
    WKPageNetworkServicesClientBase                                     base;

    // Version 0.
    WKPageNetworkServicesRequestStartedCallback                         requestStarted;
    WKPageNetworkServicesRequestFinishedCallback                        requestFinished;
    WKPageNetworkServicesRequestUpdatedCallback                         requestUpdated;
    WKPageNetworkServicesRequestCanceledCallback                        requestCanceled;
} WKPageNetworkServicesClientV0;

enum { kWKPageNetworkServicesClientCurrentVersion WK_ENUM_DEPRECATED("Use an explicit version number instead") = 0 };
typedef struct WKPageNetworkServicesClient {
    int                                                                 version;
    const void *                                                        clientInfo;

    // Version 0.
    WKPageNetworkServicesRequestStartedCallback                         requestStarted;
    WKPageNetworkServicesRequestFinishedCallback                        requestFinished;
    WKPageNetworkServicesRequestUpdatedCallback                         requestUpdated;
    WKPageNetworkServicesRequestCanceledCallback                        requestCanceled;
} WKPageNetworkServicesClient WK_DEPRECATED("Use an explicit versioned struct instead");


#ifdef __cplusplus
}
#endif


#endif // WKPageNetworkServicesClient_h
