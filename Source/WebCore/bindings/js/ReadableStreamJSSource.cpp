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
#include "ReadableStreamJSSource.h"

#if ENABLE(STREAMS_API)

#include "JSDOMPromise.h"
#include "JSReadableStream.h"
#include "NotImplemented.h"
#include "ScriptExecutionContext.h"
#include <runtime/Error.h>
#include <runtime/JSCJSValueInlines.h>
#include <runtime/JSString.h>
#include <runtime/StructureInlines.h>
#include <wtf/MainThread.h>
#include <wtf/NeverDestroyed.h>

using namespace JSC;

namespace WebCore {

void setInternalSlotToObject(ExecState* exec, JSValue objectValue, PrivateName& name, JSValue value)
{
    JSObject* object = objectValue.toObject(exec);
    PutPropertySlot propertySlot(objectValue);
    object->put(object, exec, Identifier::from(name), value, propertySlot);
}

JSValue getInternalSlotFromObject(ExecState* exec, JSValue objectValue, PrivateName& name)
{
    JSObject* object = objectValue.toObject(exec);
    PropertySlot propertySlot(objectValue);

    PropertyName propertyName = Identifier::from(name);
    if (!object->getOwnPropertySlot(object, exec, propertyName, propertySlot))
        return JSValue();
    return propertySlot.getValue(exec, propertyName);
}

// This slot name is used to store the JSReadableStream in created JS functions (enqueue, close...).
// This allows retrieving the corresponding JSReadableStream when executing the JS function.
static JSC::PrivateName& readableStreamSlotName()
{
    static NeverDestroyed<JSC::PrivateName> readableStreamSlotName("readableStream");
    return readableStreamSlotName;
}

static JSReadableStream* getJSReadableStream(ExecState* exec)
{
    JSReadableStream* jsReadableStream = jsDynamicCast<JSReadableStream*>(getInternalSlotFromObject(exec, exec->callee(), readableStreamSlotName()));
    ASSERT(jsReadableStream);
    return jsReadableStream;
}

static inline JSValue getPropertyFromObject(ExecState* exec, JSObject* object, const char* identifier)
{
    return object->get(exec, Identifier(exec, identifier));
}

JSValue ReadableStreamJSSource::callFunction(ExecState* exec, JSValue jsFunction, JSValue thisValue, const ArgList& arguments)
{
    CallData callData;
    CallType callType = getCallData(jsFunction, callData);

    JSValue exception;
    JSValue value = call(exec, jsFunction, callType, callData, thisValue, arguments, &exception);

    if (callType == CallTypeNone)
        setInternalError(exec, ASCIILiteral("Abrupt call."));

    if (exception)
        m_error.set(exec->vm(), exception);

    return value;
}

Ref<ReadableStreamJSSource> ReadableStreamJSSource::create(JSC::ExecState* exec)
{
    return adoptRef(*new ReadableStreamJSSource(exec));
}

ReadableStreamJSSource::ReadableStreamJSSource(JSC::ExecState* exec)
{
    if (!exec->argumentCount())
        return;

    if (!exec->argument(0).isObject()) {
        setInternalError(exec, ASCIILiteral("ReadableStream constructor should get an object as argument."));
        return;
    }

    m_source.set(exec->vm(), exec->argument(0).getObject());
}

void ReadableStreamJSSource::setInternalError(JSC::ExecState* exec, const String& message)
{
    m_error.set(exec->vm(), createTypeError(exec, message));
}

void ReadableStreamJSSource::storeError(JSC::ExecState* exec, JSValue error)
{
    m_error.set(exec->vm(), error);
}

const String& ReadableStreamJSSource::errorDescription() const
{
    if (!m_errorDescription && m_error) {
        ExecState* exec = m_readableStream->globalObject()->globalExec();
        const_cast<ReadableStreamJSSource*>(this)->m_errorDescription = m_error.get().toString(exec)->value(exec);
    }
    return m_errorDescription;
}

static EncodedJSValue JSC_HOST_CALL enqueueReadableStreamFunction(ExecState*)
{
    notImplemented();
    return JSValue::encode(jsUndefined());
}

static inline JSFunction* createReadableStreamEnqueueFunction(ExecState* exec)
{
    return JSFunction::create(exec->vm(), exec->callee()->globalObject(), 1, ASCIILiteral("CreateReadableStreamEnqueueFunction"), enqueueReadableStreamFunction);
}

static EncodedJSValue JSC_HOST_CALL closeReadableStreamFunction(ExecState* exec)
{
    JSReadableStream* jsReadableStream = getJSReadableStream(exec);
    jsReadableStream->impl().changeStateToClosed();
    return JSValue::encode(jsUndefined());
}

static inline JSFunction* createReadableStreamCloseFunction(ExecState* exec)
{
    return JSFunction::create(exec->vm(), exec->callee()->globalObject(), 0, ASCIILiteral("CreateReadableStreamCloseFunction"), closeReadableStreamFunction);
}

static EncodedJSValue JSC_HOST_CALL errorReadableStreamFunction(ExecState* exec)
{
    JSValue error = exec->argumentCount() ? exec->argument(0) : createError(exec, ASCIILiteral("Error function called."));
    JSReadableStream* jsReadableStream = getJSReadableStream(exec);
    static_cast<ReadableStreamJSSource&>(jsReadableStream->impl().source()).storeError(exec, error);
    jsReadableStream->impl().changeStateToErrored();
    return JSValue::encode(jsUndefined());
}

static inline JSFunction* createReadableStreamErrorFunction(ExecState* exec)
{
    return JSFunction::create(exec->vm(), exec->callee()->globalObject(), 1, ASCIILiteral("CreateReadableStreamErrorFunction"), errorReadableStreamFunction);
}

void ReadableStreamJSSource::setStream(ExecState* exec, JSReadableStream* readableStream)
{
    m_readableStream = readableStream;

    m_enqueueFunction.set(exec->vm(), createReadableStreamEnqueueFunction(exec));
    m_closeFunction.set(exec->vm(), createReadableStreamCloseFunction(exec));
    m_errorFunction.set(exec->vm(), createReadableStreamErrorFunction(exec));

    setInternalSlotToObject(exec, m_enqueueFunction.get(), readableStreamSlotName(), m_readableStream);
    setInternalSlotToObject(exec, m_closeFunction.get(), readableStreamSlotName(), m_readableStream);
    setInternalSlotToObject(exec, m_errorFunction.get(), readableStreamSlotName(), m_readableStream);
}

void ReadableStreamJSSource::startReadableStreamAsync()
{
    m_readableStream->impl().scriptExecutionContext()->postTask([this](ScriptExecutionContext&) {
        m_readableStream->impl().unsetPendingActivity(&m_readableStream->impl());
        m_readableStream->impl().start();
    });
}

void ReadableStreamJSSource::start(JSC::ExecState* exec)
{
    m_readableStream->impl().setPendingActivity(&m_readableStream->impl());
    if (!m_source) {
        startReadableStreamAsync();
        return;
    }
    JSLockHolder lock(exec);
    JSValue startFunction = getPropertyFromObject(exec, m_source.get(), "start");
    if (!startFunction.isFunction()) {
        if (!startFunction.isUndefined())
            throwVMError(exec, createTypeError(exec, ASCIILiteral("ReadableStream constructor object start property should be a function.")));
        else
            startReadableStreamAsync();
        return;
    }

    MarkedArgumentBuffer arguments;
    arguments.append(m_enqueueFunction.get());
    arguments.append(m_closeFunction.get());
    arguments.append(m_errorFunction.get());

    callFunction(exec, startFunction, m_source.get(), arguments);

    // Throw except if m_errorFunction was called.
    if (m_error && !m_readableStream->impl().isErrored()) {
        throwVMError(exec, m_error.get());
        return;
    }

    // FIXME: Implement handling promise as result of calling start function.
    startReadableStreamAsync();
}

} // namespace WebCore

#endif
