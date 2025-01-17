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

#ifndef ReadableStream_h
#define ReadableStream_h

#if ENABLE(STREAMS_API)

#include "ActiveDOMObject.h"
#include "ReadableStreamSource.h"
#include "ScriptWrappable.h"
#include "SharedBuffer.h"
#include <functional>
#include <runtime/JSCJSValue.h>
#include <wtf/Ref.h>
#include <wtf/Vector.h>

namespace WebCore {

class JSDOMGlobalObject;
class ScriptExecutionContext;

// ReadableStream implements the core of the streams API ReadableStream functionality.
// It handles in particular the backpressure according the queue size.
// ReadableStream is using a ReadableStreamSource to get data in its queue.
// See https://streams.spec.whatwg.org/#rs
class ReadableStream : public ActiveDOMObject, public ScriptWrappable, public RefCounted<ReadableStream> {
public:
    enum class State {
        Waiting,
        Closed,
        Readable,
        Errored
    };

    virtual ~ReadableStream();

    // JS API implementation.
    virtual JSC::JSValue read(JSC::ExecState*, JSDOMGlobalObject*) = 0;
    String state() const;

    typedef std::function<void()> SuccessCallback;
    typedef std::function<void()> ErrorCallback;

    void closed(SuccessCallback, ErrorCallback);
    void ready(SuccessCallback);
    void cancel(const String&, SuccessCallback, ErrorCallback);

    bool isErrored() { return m_state == State::Errored; }
    bool isReadable() { return m_state == State::Readable; }
    bool isWaiting() { return m_state == State::Waiting; }
    bool isClosed() { return m_state == State::Closed; }

    // API used from the JS binding.
    void start();

    void changeStateToClosed();
    virtual void changeStateToErrored();
    void notifyCancelSucceeded();
    void notifyCancelFailed();

    ReadableStreamSource& source() { return m_source.get(); }
    bool canEnqueue(String&);
    void finishPulling();
protected:
    ReadableStream(ScriptExecutionContext&, Ref<ReadableStreamSource>&&);

    bool enqueueing(unsigned);
    void dequeueing(unsigned);

private:
    void resolveClosedCallback();
    void resolveReadyCallback();
    void rejectClosedCallback();
    bool shouldApplyBackpressure();
    void callReadableStreamPull();
    bool canPull();

    // ActiveDOMObject API.
    const char* activeDOMObjectName() const override;
    bool canSuspend() const override;

    State m_state;
    Ref<ReadableStreamSource> m_source;

    SuccessCallback m_closedSuccessCallback;
    ErrorCallback m_closedErrorCallback;

    SuccessCallback m_readyCallback;
    SuccessCallback m_cancelledSuccessCallback;
    ErrorCallback m_cancelledErrorCallback;

    unsigned m_totalQueueSize { 0 };

    bool m_isDraining { false };
    bool m_isPulling { false };
    bool m_isPullScheduled { false };
    bool m_isStarted { false };
};

class ReadableArrayBufferStream: public ReadableStream
{
public: 
    ReadableArrayBufferStream(ScriptExecutionContext& scriptExecutionContext, Ref<ReadableStreamSource>&& source) : ReadableStream(scriptExecutionContext, WTF::move(source)) { }
    JSC::JSValue read(JSC::ExecState*, JSDOMGlobalObject*);
    void enqueue(PassRefPtr<SharedBuffer>);
    void changeStateToErrored();
private:
    Vector<RefPtr<SharedBuffer>> m_queue;
};

}

#endif

#endif // ReadableStream_h
