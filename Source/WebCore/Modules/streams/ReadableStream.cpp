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
#include "ReadableStream.h"

#if ENABLE(STREAMS_API)

#include "ScriptExecutionContext.h"
#include <wtf/MainThread.h>
#include <wtf/RefCountedLeakCounter.h>

namespace WebCore {

DEFINE_DEBUG_ONLY_GLOBAL(WTF::RefCountedLeakCounter, readableStreamCounter, ("ReadableStream"));

ReadableStream::ReadableStream(ScriptExecutionContext& scriptExecutionContext, Ref<ReadableStreamSource>&& source)
    : ActiveDOMObject(&scriptExecutionContext)
    , m_state(State::Waiting)
    , m_source(WTF::move(source))
{
#ifndef NDEBUG
    readableStreamCounter.increment();
#endif
}

ReadableStream::~ReadableStream()
{
#ifndef NDEBUG
    readableStreamCounter.decrement();
#endif
}

String ReadableStream::state() const
{
    switch (m_state) {
    case State::Waiting:
        return ASCIILiteral("waiting");
    case State::Closed:
        return ASCIILiteral("closed");
    case State::Readable:
        return ASCIILiteral("readable");
    case State::Errored:
        return ASCIILiteral("errored");
    }
    ASSERT_NOT_REACHED();
    return String();
}

void ReadableStream::resolveClosedCallback()
{
    ASSERT(m_state == State::Closed);
    if (!m_closedSuccessCallback)
        return;

    m_closedErrorCallback = nullptr;
    SuccessCallback closedSuccessCallback = WTF::move(m_closedSuccessCallback);
    closedSuccessCallback();
}

void ReadableStream::rejectClosedCallback()
{
    ASSERT(m_state == State::Errored);
    if (!m_closedErrorCallback)
        return;
    m_closedSuccessCallback = nullptr;
    ErrorCallback closedErrorCallback = WTF::move(m_closedErrorCallback);
    closedErrorCallback();
}

void ReadableStream::closed(SuccessCallback successCallback, ErrorCallback errorCallback)
{
    if (m_state == State::Closed) {
        successCallback();
        return;
    }
    else if (m_state == State::Errored) {
        errorCallback();
        return;
    }
    m_closedSuccessCallback = WTF::move(successCallback);
    m_closedErrorCallback = WTF::move(errorCallback);
}

void ReadableStream::changeStateToClosed()
{
    if (m_state == State::Waiting) {
        m_state = State::Closed;
        resolveReadyCallback();
        resolveClosedCallback();
        unsetPendingActivity(this);
    } else if (m_state == State::Readable)
        m_isDraining = true;
}

void ReadableStream::changeStateToErrored()
{
    if (m_state == State::Errored || m_state == State::Closed)
        return;
    m_totalQueueSize = 0;
    m_state = State::Errored;
    resolveReadyCallback();
    rejectClosedCallback();
    unsetPendingActivity(this);
}

void ReadableStream::resolveReadyCallback()
{
    ASSERT(m_state != State::Waiting);
    if (!m_readyCallback)
        return;
    SuccessCallback readyCallback = WTF::move(m_readyCallback);
    readyCallback();
}

void ReadableStream::ready(SuccessCallback callback)
{
    if (m_state != State::Waiting) {
        callback();
        return;
    }
    m_readyCallback = WTF::move(callback);
}

void ReadableStream::notifyCancelSucceeded()
{
    if (!m_cancelledSuccessCallback)
        return;
    SuccessCallback cancelledSuccessCallback = WTF::move(m_cancelledSuccessCallback);
    cancelledSuccessCallback();
    unsetPendingActivity(this);
}

void ReadableStream::notifyCancelFailed()
{
    if (!m_cancelledErrorCallback)
        return;
    ErrorCallback cancelledErrorCallback = WTF::move(m_cancelledErrorCallback);
    cancelledErrorCallback();
    unsetPendingActivity(this);
}

void ReadableStream::cancel(const String& reason, SuccessCallback successCallback, ErrorCallback errorCallback)
{
    if (m_state == State::Closed) {
        successCallback();
        return;
    }
    else if (m_state == State::Errored) {
        errorCallback();
        return;
    }
    else if (m_state == State::Waiting)
        resolveReadyCallback();

    m_state = State::Closed;
    resolveClosedCallback();

    m_totalQueueSize = 0;

    m_cancelledSuccessCallback = WTF::move(successCallback);
    m_cancelledErrorCallback = WTF::move(errorCallback);
    if (!m_source->cancel(reason)) {
        // The underlying source is responsible to call one of the notifyCancel methods.
        setPendingActivity(this);
        return;
    }
    notifyCancelSucceeded();
}

void ReadableStream::start()
{
    setPendingActivity(this);
    m_isStarted = true;
    callReadableStreamPull();
}

bool ReadableStream::canPull()
{
    return !m_isDraining && m_isStarted && m_state != State::Closed && m_state != State::Errored;
}

void ReadableStream::callReadableStreamPull()
{
    if (!canPull())
        return;

    if (shouldApplyBackpressure())
        return;

    if (m_isPulling) {
        m_isPullScheduled = true;
        return;
    }

    m_isPulling = true;
    if (m_source->pull())
        finishPulling();
}

void ReadableStream::finishPulling()
{
    m_isPulling = false;
    if (m_isPullScheduled) {
        m_isPullScheduled = false;
        callReadableStreamPull();
    }
}

const char* ReadableStream::activeDOMObjectName() const
{
    return "ReadableStream";
}

bool ReadableStream::canSuspend() const
{
    // FIXME: We should try and do better here.
    return false;
}

void ReadableStream::dequeueing(unsigned size)
{
    ASSERT(m_state == State::Readable);
    ASSERT(m_totalQueueSize > 0);

    m_totalQueueSize = m_totalQueueSize - size;

    if (!m_totalQueueSize) {
        if (m_isDraining) {
            m_state = State::Closed;
            resolveClosedCallback();
        } else
            m_state = State::Waiting;
    }

    callReadableStreamPull();
}

bool ReadableStream::shouldApplyBackpressure()
{
    bool shouldApplyBackpressureAsBool = m_source->shouldApplyBackpressure(m_totalQueueSize);

    // In case of rethrow, stop pulling and exit, hence returning true here.
    if (m_source->isErrored())
        return true;

    return shouldApplyBackpressureAsBool;
}

bool ReadableStream::canEnqueue(String &error)
{
    if (m_state == State::Errored) {
        error = m_source->errorDescription();
        return false;
    }

    if (m_state == State::Closed) {
        error = ASCIILiteral("tried to enqueue data in a closed stream");
        return false;
    }

    if (m_isDraining) {
        error = ASCIILiteral("tried to enqueue data in a draining stream");
        return false;
    }
    return true;
}

bool ReadableStream::enqueueing(unsigned chunkSize)
{
    m_state = State::Readable;

    m_totalQueueSize += chunkSize;

    resolveReadyCallback();

    return shouldApplyBackpressure();
}

JSC::JSValue ReadableArrayBufferStream::read(JSC::ExecState* exec, JSDOMGlobalObject* globalObject)
{
    RefPtr<SharedBuffer> buffer = m_queue.takeLast();
    dequeueing(1);
    return toJS(exec, globalObject, buffer->createArrayBuffer());
}

void ReadableArrayBufferStream::enqueue(PassRefPtr<SharedBuffer> buffer)
{
    m_queue.insert(0, buffer);
    enqueueing(1);
}

void ReadableArrayBufferStream::changeStateToErrored()
{
    m_queue.shrink(0);
    ReadableStream::changeStateToErrored();
}

}

#endif
