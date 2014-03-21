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
#include "NavigatorNetworkService.h"

#if ENABLE(DISCOVERY)

#include "CrossThreadTask.h"
#include "DOMRequestState.h"
#include "Document.h"
#include "ExceptionCode.h"
#include "Frame.h"
#include "Navigator.h"
#include "NetworkServicesController.h"
#include "NetworkServicesManager.h"
#include "URIBindingUtilities.h"

namespace WebCore {

static bool validType(const char* type)
{
    char* ptr;
    char c;
    size_t len;

    if (strncmp(type, "upnp:", 5) && strncmp(type, "zeroconf:", 9))
        return false;

    ptr = strchr((char *)type, ':');
    ptr++;
    len = strlen(ptr);

    /* check that all characters are in the ranges
        ^,_,`,[a,z],{,|,},~ : U+005E to U+007E
        [A-Z]               : U+0041 to U+005A, 
        [0,9]               : U+0030 to U+0039,
        -,.                 : U+002D to U+002E, 
        *,+                 : U+002A to U+002B, 
        #,$,%,&,'           : U+0023 to U+0027, 
        '!'                 : U+0021, 
    */
    while (len--) {
        c = *ptr++;
        if ((c >= 0x5E && c <= 0x7E) 
            || (c >= 0x41 && c <= 0x5A) 
            || (c >= 0x30 && c <= 0x39) 
            || (c >= 0x2D && c <= 0x2E) 
            || (c >= 0x2A && c <= 0x2B)
            || (c >= 0x23 && c <= 0x27)
            || (c == 0x3A) // add ':' needed for upnp URLs?
            || (c == 0x21)) {
            continue;
        }
        return false;
    }
    return true;
}

static void cleanTypes(Vector<String> &types)
{
    size_t i = 0;
    
    while (i < types.size()) {
        
        String type = types[i++];

        if (type.startsWith(String("upnp:"))
            || type.startsWith(String("zeroconf:"))) {
            if (!validType(type.utf8().data()))
                types.remove(--i);
        // } else if (type.startsWith(String("dial:"))) {
        } else
            types.remove(--i);
    }
}

NavigatorNetworkService::NavigatorNetworkService()
    : m_networkServicesManager(0)
{
}

NavigatorNetworkService::~NavigatorNetworkService()
{
}

const char* NavigatorNetworkService::supplementName()
{
    return "NavigatorNetworkService";
}

NavigatorNetworkService* NavigatorNetworkService::from(Navigator* navigator)
{
    NavigatorNetworkService* supplement = static_cast<NavigatorNetworkService*>(Supplement<Navigator>::from(navigator, supplementName()));
    if (!supplement) {
        supplement = new NavigatorNetworkService();
        provideTo(navigator, supplementName(), adoptPtr(supplement));
    }
    return supplement;
}

void NavigatorNetworkService::getNetworkServices(Navigator* navigator,
    Deprecated::ScriptValue& type,
    PassRefPtr<NavigatorNetworkServiceSuccessCallback> successCallback,
    PassRefPtr<NavigatorNetworkServiceErrorCallback> errorCallback,
    ExceptionCode& ec)
{
    NavigatorNetworkService* navigatorService = NavigatorNetworkService::from(navigator);
    NetworkServicesController* controller = NetworkServicesController::from(navigator->frame() ? navigator->frame()->page() : 0);
    
    if (!controller) {
        ec = NOT_SUPPORTED_ERR;
        return;
    }

    if (!successCallback)
        return;

    ScriptExecutionContext* c = navigator->frame()->document();
    Vector<String> types = getRequestedTypes(c, type);

    cleanTypes(types);

    if (!types.size()) {
        if (errorCallback)
            scheduleCallback(c, errorCallback, NavigatorNetworkServiceError::create(NavigatorNetworkServiceError::UNKNOWN_TYPE_PREFIX_ERR)); 
        return;
    }

    if (!navigatorService->m_networkServicesManager)
        navigatorService->m_networkServicesManager = NetworkServicesManager::create(c, successCallback, errorCallback, types);
    else
        navigatorService->m_networkServicesManager->update(successCallback, errorCallback, types);
}

Vector<String> NavigatorNetworkService::getRequestedTypes(ScriptExecutionContext *c, Deprecated::ScriptValue& type)
{
    DOMRequestState requestState = DOMRequestState(c);
    return getURIVectorFromScriptValue(&requestState, type);
}

} // namespace WebCore

#endif // ENABLE(DISCOVERY)
