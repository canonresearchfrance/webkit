/*
 * Copyright (C) 2015 Canon Inc.
 * Copyright (C) 2015 Igalia S.L.
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

#if ENABLE(STREAMS_API)
#include "JSReadableStream.h"

#include "ExceptionCode.h"
#include "JSDOMPromise.h"
#include "ReadableStream.h"
#include "ReadableStreamJSSource.h"
#include <wtf/NeverDestroyed.h>

using namespace JSC;

namespace WebCore {

static JSC::PrivateName& readyPromiseSlotName();
static inline void clearReadyPromise(ExecState* exec, const JSReadableStream* stream)
{
    setInternalSlotToObject(exec, stream, readyPromiseSlotName(), jsUndefined());
}

JSValue JSReadableStream::read(ExecState* exec)
{
    if (!impl().isReadable()) {
        if (impl().isErrored()) {
            JSValue error = impl().source().isJS() ? static_cast<ReadableStreamJSSource&>(impl().source()).error() : createTypeError(exec, impl().source().errorDescription()); 
            return exec->vm().throwException(exec, error);
        }
        setDOMException(exec, TypeError);
        return jsUndefined();
    }

    // Whenever transitioning to State::Waiting, we need to create a new ready promise.
    JSValue value = impl().read(exec, globalObject());
    if (impl().isWaiting())
        clearReadyPromise(exec, this);
    return value;
}

static JSPromiseDeferred* getOrCreatePromiseDeferredFromObject(ExecState* exec, JSValue thisObject, JSGlobalObject* globalObject, PrivateName &name)
{
    JSValue slot = getInternalSlotFromObject(exec, thisObject, name);
    JSPromiseDeferred* promiseDeferred = slot && !slot.isUndefined()? jsDynamicCast<JSPromiseDeferred*>(slot) : nullptr;

    if (!promiseDeferred) {
        promiseDeferred = JSPromiseDeferred::create(exec, globalObject);
        setInternalSlotToObject(exec, thisObject, name, promiseDeferred);
    }
    return promiseDeferred;
}

static JSC::PrivateName& readyPromiseSlotName()
{
    static NeverDestroyed<JSC::PrivateName> readyPromiseSlotName("readyPromise");
    return readyPromiseSlotName;
}

JSValue JSReadableStream::ready(ExecState* exec) const
{
    JSPromiseDeferred* promiseDeferred = getOrCreatePromiseDeferredFromObject(exec, this, globalObject(), readyPromiseSlotName());
    DeferredWrapper wrapper(exec, globalObject(), promiseDeferred);
    auto successCallback = [wrapper]() mutable {
        wrapper.resolve(jsUndefined());
    };

    impl().ready(WTF::move(successCallback));

    return wrapper.promise();
}

static JSC::PrivateName& closedPromiseSlotName()
{
    static NeverDestroyed<JSC::PrivateName> closedPromiseSlotName("closedPromise");
    return closedPromiseSlotName;
}

JSValue JSReadableStream::closed(ExecState* exec) const
{
    JSPromiseDeferred* promiseDeferred = getOrCreatePromiseDeferredFromObject(exec, this, globalObject(), closedPromiseSlotName());
    DeferredWrapper wrapper(exec, globalObject(), promiseDeferred);
    auto successCallback = [wrapper]() mutable {
        wrapper.resolve(jsUndefined());
    };
    auto failureCallback = [this, wrapper]() mutable {
        JSValue error = impl().source().isJS() ? static_cast<ReadableStreamJSSource&>(impl().source()).error() : createError(globalObject()->globalExec(), impl().source().errorDescription());
        wrapper.reject(error);
    };

    impl().closed(WTF::move(successCallback), WTF::move(failureCallback));

    return wrapper.promise();
}

JSValue JSReadableStream::cancel(ExecState* exec)
{
    if (impl().isClosed() || impl().isErrored())
        return closed(exec);

    DeferredWrapper wrapper(exec, globalObject());
    auto successCallback = [wrapper]() mutable {
        wrapper.resolve(jsUndefined());
    };
    auto failureCallback = [this, wrapper]() mutable {
        if (impl().source().isJS()) {
            JSValue jsError = static_cast<ReadableStreamJSSource&>(impl().source()).error();
            wrapper.reject(jsError);
        } else
            wrapper.reject(impl().source().errorDescription());
    };

    JSValue reason = exec->argument(0);
    if (impl().source().isJS())
        static_cast<ReadableStreamJSSource&>(impl().source()).willCancel(exec, reason);
    impl().cancel(reason.toString(exec)->value(exec), WTF::move(successCallback), WTF::move(failureCallback));

    return wrapper.promise();
}

JSValue JSReadableStream::pipeTo(ExecState* exec)
{
    JSValue error = createError(exec, ASCIILiteral("pipeTo is not implemented"));
    return exec->vm().throwException(exec, error);
}

JSValue JSReadableStream::pipeThrough(ExecState* exec)
{
    JSValue error = createError(exec, ASCIILiteral("pipeThrough is not implemented"));
    return exec->vm().throwException(exec, error);
}

EncodedJSValue JSC_HOST_CALL constructJSReadableStream(ExecState* exec)
{
    DOMConstructorObject* jsConstructor = jsCast<DOMConstructorObject*>(exec->callee());
    ASSERT(jsConstructor);
    ScriptExecutionContext* scriptExecutionContext = jsConstructor->scriptExecutionContext();

    Ref<ReadableStreamJSSource> source = ReadableStreamJSSource::create(exec);
    if (source->isErrored())
        return throwVMError(exec, source->error());

    RefPtr<ReadableStream> readableStream = ReadableJSValueStream::create(*scriptExecutionContext, source.get());

    VM& vm = exec->vm();
    JSGlobalObject* globalObject = exec->callee()->globalObject();
    JSReadableStream* jsReadableStream = JSReadableStream::create(JSReadableStream::createStructure(vm, globalObject, JSReadableStream::createPrototype(vm, globalObject)), jsCast<JSDOMGlobalObject*>(globalObject), readableStream.releaseNonNull());

    source->setStream(exec, jsReadableStream);
    source->start(exec);

    return JSValue::encode(jsReadableStream);
}

} // namespace WebCore

#endif
