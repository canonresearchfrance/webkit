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

Ref<ReadableStream> ReadableStream::create(ScriptExecutionContext& scriptExecutionContext, Ref<ReadableStreamSource>&& source)
{
    auto readableStream = adoptRef(*new ReadableStream(scriptExecutionContext, WTF::move(source)));
    readableStream.get().suspendIfNeeded();

    return readableStream;
}

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
    }
    // FIXME: Handle State::Readable when enqueue is supported.
}

void ReadableStream::changeStateToErrored()
{
    if (m_state == State::Errored || m_state == State::Closed)
        return;
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

void ReadableStream::start()
{
    setPendingActivity(this);
    // FIXME: Implement pulling data
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

}

#endif
