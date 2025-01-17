/*
 * Copyright (C) 2O15 Canon Inc. 2015
 * Copyright (C) 2015 Igalia S.L. 2015
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

#ifndef ReadableStreamJSSource_h
#define ReadableStreamJSSource_h

#if ENABLE(STREAMS_API)

#include "ReadableStream.h"
#include "ReadableStreamSource.h"
#include <heap/Strong.h>
#include <heap/StrongInlines.h>
#include <runtime/JSCJSValue.h>
#include <runtime/JSFunction.h>
#include <runtime/PrivateName.h>
#include <wtf/Ref.h>
#include <wtf/Vector.h>

namespace WebCore {

class JSReadableStream;
class ReadableJSValueStream;

class ReadableStreamJSSource: public ReadableStreamSource {
public:
    static Ref<ReadableStreamJSSource> create(JSC::ExecState*);
    ~ReadableStreamJSSource() { }

    void setStream(JSC::ExecState*, JSReadableStream*);

    JSC::JSValue error() { return m_error.get(); }
    void storeError(JSC::ExecState*, JSC::JSValue);
    void start(JSC::ExecState*);

    unsigned chunkSize(JSC::ExecState*, JSC::JSValue);
    void enqueue(JSC::JSValue);
    JSC::JSValue read();
    void willCancel(JSC::ExecState*, JSC::JSValue);

    // ReadableStreamSource API.
    virtual bool isErrored() override { return !!m_error; }
    virtual bool isJS() const override { return true; }
    virtual const String& errorDescription() const override;
    virtual bool shouldApplyBackpressure(unsigned) override;
    virtual bool pull() override;
    virtual bool cancel(const String&) override;

private:
    void setInternalError(JSC::ExecState*, const String&);

    ReadableStreamJSSource(JSC::ExecState*);
    JSC::JSValue callFunction(JSC::ExecState*, JSC::JSValue, JSC::JSValue, const JSC::ArgList&);
    void startReadableStreamAsync();

    // m_error may be an error generated from ReadableStreamJSSource or from JS callbacks.
    JSC::Strong<JSC::Unknown> m_error;
    String m_errorDescription;
    JSReadableStream* m_readableStream { nullptr };

    // Object passed to constructor.
    JSC::Strong<JSC::JSObject> m_source;

    // Functions passed as parameters when calling the functions passed in the constructor.
    JSC::Strong<JSC::JSFunction> m_enqueueFunction;
    JSC::Strong<JSC::JSFunction> m_closeFunction;
    // FIXME: Decide whether creating the error function on the fly when calling the start source function.
    JSC::Strong<JSC::JSFunction> m_errorFunction;

    JSC::Strong<JSC::Unknown> m_cancelReason;
};

void setInternalSlotToObject(JSC::ExecState*, JSC::JSValue, JSC::PrivateName&, JSC::JSValue);
JSC::JSValue getInternalSlotFromObject(JSC::ExecState*, JSC::JSValue, JSC::PrivateName&);

class ReadableJSValueStream: public ReadableStream {
public:
    JSC::JSValue read(JSC::ExecState*, JSDOMGlobalObject*) override;
    bool enqueue(JSC::ExecState*, JSC::JSValue, unsigned);
    void changeStateToErrored() override;

    static Ref<ReadableJSValueStream> create(ScriptExecutionContext&, Ref<ReadableStreamJSSource>&&);
    ReadableJSValueStream(ScriptExecutionContext& scriptExecutionContext, Ref<ReadableStreamJSSource>&& source) : ReadableStream(scriptExecutionContext, Ref<ReadableStreamSource>(source.get())) { }
private:
    Vector<unsigned> m_sizeQueue;
    Vector<JSC::Strong<JSC::Unknown>> m_chunkQueue;
};

} // namespace WebCore

#endif // ENABLE(STREAMS_API)

#endif // ReadableStreamJSSource_h
