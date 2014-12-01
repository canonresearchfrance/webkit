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
#ifndef ResourceResolver_h
#define ResourceResolver_h

#include "ResourceHandleTypes.h"

#include <wtf/PassRefPtr.h>
#include <wtf/RefCounted.h>
#include <wtf/Vector.h>

namespace WebCore {

class NetworkingContext;
class ResourceError;
class ResourceHandle;
class ResourceHandleClient;
class ResourceRequest;
class ResourceResponse;
class ResourceResolverAsync;
class ResourceResolverAsyncClient;
class ResourceResolverClient;

class ResourceResolver : public RefCounted<ResourceResolver> {

public:
    // FIXME: Replace the use of ResourceHandle create and loadResourceSynchronously methods by these two methods.
    // Async client and handle client are optional.
    WEBCORE_EXPORT static PassRefPtr<ResourceResolver> create(NetworkingContext*, const ResourceRequest&, ResourceResolverClient*, ResourceResolverAsyncClient*, ResourceHandleClient*, bool, bool);
    WEBCORE_EXPORT static void loadResourceSynchronously(NetworkingContext*, const ResourceRequest&, StoredCredentials, ResourceError&, ResourceResponse&, Vector<char>&);

    virtual ~ResourceResolver() { }

    virtual void cancel() { }
    virtual void setDefersLoading(bool) { }

    // FIXME: We should probably remove these methods whenever possible.
    virtual ResourceResolverAsync* async() { return nullptr; }
    virtual ResourceHandle* handle() { return nullptr; }

    // FIXME: Can we merge clearClient/isClient with the cancel method?
    virtual void clearClient() = 0;
    virtual bool isClient(ResourceResolverClient*) const = 0;
    virtual ResourceRequest& firstRequest() = 0;
};

}

#endif // ResourceResolver_h
