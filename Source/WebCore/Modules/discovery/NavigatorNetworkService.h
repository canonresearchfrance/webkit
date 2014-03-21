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

#ifndef NavigatorNetworkService_h
#define NavigatorNetworkService_h

#if ENABLE(DISCOVERY)

#include "NavigatorNetworkServiceError.h"
#include "NavigatorNetworkServiceErrorCallback.h"
#include "NavigatorNetworkServiceSuccessCallback.h"
#include "ScriptExecutionContext.h"
#include "Supplementable.h"
#include <bindings/ScriptValue.h>
#include <wtf/PassRefPtr.h>
#include <wtf/RefPtr.h>
#include <wtf/text/WTFString.h>

namespace WebCore {

class Document;
class Frame;
class NetworkServicesController;
class NetworkServicesManager;
class NavigatorNetworkServiceSuccessCallback;
class NavigatorNetworkServiceErrorCallback;
class Navigator;

typedef int ExceptionCode;

class NavigatorNetworkService : public Supplement<Navigator> {
public:
    virtual ~NavigatorNetworkService();

    static const char* supplementName();
    static NavigatorNetworkService* from(Navigator*);

    static void getNetworkServices(Navigator*, Deprecated::ScriptValue&, PassRefPtr<NavigatorNetworkServiceSuccessCallback>, PassRefPtr<NavigatorNetworkServiceErrorCallback>, ExceptionCode&);

    // Schedule a callback. This should not cross threads (should be called on the same context thread).
    // FIXME: move this to a more generic place.
    template <typename CB, typename CBArg>
    static void scheduleCallback(ScriptExecutionContext*, PassRefPtr<CB>, PassRefPtr<CBArg>);

private:
    // A helper template to schedule a callback task.
    template <typename CB, typename CBArg>
    class DispatchCallbackTask : public ScriptExecutionContext::Task {
    public:
        DispatchCallbackTask(PassRefPtr<CB> callback, PassRefPtr<CBArg> arg)
            : m_callback(callback)
            , m_callbackArg(arg)
        {
        }

        virtual void performTask(ScriptExecutionContext*)
        {
            m_callback->handleEvent(m_callbackArg.get());
        }

    private:
        RefPtr<CB> m_callback;
        RefPtr<CBArg> m_callbackArg;
    };

    static Vector<String> getRequestedTypes(ScriptExecutionContext *, Deprecated::ScriptValue&);

    explicit NavigatorNetworkService();

    RefPtr<NetworkServicesManager> m_networkServicesManager;
};

template <typename CB, typename CBArg>
void NavigatorNetworkService::scheduleCallback(ScriptExecutionContext* scriptExecutionContext, PassRefPtr<CB> callback, PassRefPtr<CBArg> arg)
{
    ASSERT(scriptExecutionContext->isContextThread());
    if (callback)
        scriptExecutionContext->postTask(adoptPtr(new DispatchCallbackTask<CB, CBArg>(callback, arg)));
}

} // namespace WebCore

#endif // ENABLE(DISCOVERY)

#endif // NavigatorNetworkService_h
