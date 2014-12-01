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

#ifndef ResourceResolverClient_h
#define ResourceResolverClient_h

#include "SharedBuffer.h"
#include <wtf/PassRefPtr.h>
#include <wtf/Vector.h>

namespace WebCore {

class ResourceError;
class ResourceRequest;
class ResourceResolver;
class ResourceResponse;

class ResourceResolverClient {
public:
    virtual ~ResourceResolverClient() { }

    // Request may be modified.
    virtual void willSendRequest(ResourceResolver*, ResourceRequest&, const ResourceResponse& /*redirectResponse*/) { }
    virtual void didSendData(ResourceResolver*, unsigned long long /*bytesSent*/, unsigned long long /*totalBytesToBeSent*/) { }

    virtual void didReceiveResponse(ResourceResolver*, const ResourceResponse&) { }

    virtual void didReceiveData(ResourceResolver*, const char*, unsigned, int /*encodedDataLength*/) { }
    virtual void didReceiveBuffer(ResourceResolver* resolver, PassRefPtr<SharedBuffer> buffer, int encodedDataLength) { didReceiveData(resolver, buffer->data(), buffer->size(), encodedDataLength); }

    virtual void didFinishLoading(ResourceResolver*, double /*finishTime*/) { }

    virtual void didFail(ResourceResolver*, const ResourceError&) { }
    // FIXME: Should we remove wasBlocked and cannotShowURL and always use didFail instead?
    virtual void wasBlocked(ResourceResolver*) { }
    virtual void cannotShowURL(ResourceResolver*) { }

    virtual bool shouldUseCredentialStorage(ResourceResolver*) { return false; }
};

}

#endif // ResourceResolverClient_h
