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
#include "ResourceResolver.h"

#include "BlobResourceHandle.h"
#include "ResourceHandle.h"
#include "ResourceHandleClient.h"
#include "ResourceRequest.h"
#include "ResourceResolverAsync.h"
#include "ResourceResolverClient.h"
#include "wtf/NeverDestroyed.h"

namespace WebCore {

static ResourceHandleClient* defaultClient()
{
    static NeverDestroyed<ResourceHandleClient> client;
    return &client.get();
}

PassRefPtr<ResourceResolver> ResourceResolver::create(NetworkingContext* context, const ResourceRequest& request, ResourceResolverClient* client, ResourceResolverAsyncClient* asyncClient, ResourceHandleClient* handleClient, bool defersLoading, bool shouldContentSniff)
{
    if (request.url().protocolIs("blob")) {
        RefPtr<BlobResourceHandle> resolver = BlobResourceHandle::create(request, client, asyncClient);
        resolver->start();
        return resolver;
    }

    return ResourceHandle::create(context, request, client, asyncClient, handleClient ? handleClient : defaultClient(), defersLoading, shouldContentSniff);
}

void ResourceResolver::loadResourceSynchronously(NetworkingContext* context, const ResourceRequest& request, StoredCredentials storedCredentials, ResourceError& error, ResourceResponse& response, Vector<char>& data)
{
    if (request.url().protocolIs("blob")) {
        BlobResourceHandle::loadResourceSynchronously(request, error, response, data);
        return;
    }
    ResourceHandle::loadResourceSynchronously(context, request, storedCredentials, error, response, data);
}

}
