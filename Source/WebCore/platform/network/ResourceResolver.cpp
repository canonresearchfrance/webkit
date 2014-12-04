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

namespace WebCore {

// FIXME: Merge this class with ResourceHandle once ResourceHandle::create is no longer called by other classes than ResourceResolver.
class ResourceHandleResolver: public ResourceResolverAsync, private ResourceHandleClient, private ResourceResolverAsyncClient {
public:

    // ResourceHandleClient API forward to ResourceResolver client and async client.
    void willSendRequest(ResourceHandle*, ResourceRequest& request, const ResourceResponse& redirectResponse)
    {
        if (m_client)
            m_client->willSendRequest(this, request, redirectResponse);
    }
    void didSendData(ResourceHandle*, unsigned long long bytesSent, unsigned long long totalBytesToBeSent)
    {
        if (m_client)
            m_client->didSendData(this, bytesSent, totalBytesToBeSent);
    }

    void didReceiveResponse(ResourceHandle*, const ResourceResponse& response)
    {
        if (m_client)
            m_client->didReceiveResponse(this, response);
    }

    void didReceiveData(ResourceHandle*, const char* data, unsigned length, int encodedDataLength)
    {
        if (m_client)
            m_client->didReceiveData(this, data, length, encodedDataLength);
    }
    void didReceiveBuffer(ResourceHandle*, PassRefPtr<SharedBuffer> buffer, int encodedDataLength)
    {
        if (m_client)
            m_client->didReceiveBuffer(this, buffer, encodedDataLength);
    }

    void didFinishLoading(ResourceHandle*, double finishTime)
    {
        if (m_client)
            m_client->didFinishLoading(this, finishTime);
    }

    void didFail(ResourceHandle*, const ResourceError& error)
    {
        if (m_client)
            m_client->didFail(this, error);
    }
    void wasBlocked(ResourceHandle*)
    {
        if (m_client)
            m_client->wasBlocked(this);
    }

    void cannotShowURL(ResourceHandle*)
    {
        if (m_client)
            m_client->cannotShowURL(this);
    }

    using ResourceResolverAsyncClient::didReceiveResponseAsync;
    void didReceiveResponseAsync(ResourceHandle*, const ResourceResponse& response)
    {
        if (m_asyncClient)
            m_asyncClient->didReceiveResponseAsync(this, response);
    }

    bool shouldUseCredentialStorage(ResourceHandle*) { return m_client->shouldUseCredentialStorage(this); }

    // ResourceHandleClient API forward to handle client.
#if USE(PROTECTION_SPACE_AUTH_CALLBACK)
        void canAuthenticateAgainstProtectionSpaceAsync(ResourceHandle* handle, const ProtectionSpace& space)
        {
            if (m_handleClient)
                m_handleClient->canAuthenticateAgainstProtectionSpaceAsync(handle, space);
        }
#endif
#if USE(CFNETWORK)
        void willCacheResponseAsync(ResourceHandle* handle, CFCachedURLResponseRef ref)
        {
            if (m_handleClient)
                m_handleClient->willCacheResponseAsync(handle, ref);
        }
#elif PLATFORM(COCOA)
        void willCacheResponseAsync(ResourceHandle* handle, NSCachedURLResponse* response)
        {
            if (m_handleClient)
                m_handleClient->willCacheResponseAsync(handle, response);
        }
#endif

#if USE(NETWORK_CFDATA_ARRAY_CALLBACK)
        bool supportsDataArray()  { return m_handleClient ? m_handleClient->supportsDataArray() : false; }
        void didReceiveDataArray(ResourceHandle* handle, CFArrayRef ref)
        {
            if (m_handleClient)
                m_handleClient->didReceiveDataArray(handle, ref);
        }
#endif

#if USE(SOUP)
        char* getOrCreateReadBuffer(size_t requestedLength, size_t& actualLength)
        {
            return m_handleClient ? m_handleClient->getOrCreateReadBuffer(requestedLength, actualLength) : nullptr;
        }
#endif

        void didReceiveAuthenticationChallenge(ResourceHandle* handle, const AuthenticationChallenge& challenge)
        {
            if (m_handleClient)
                m_handleClient->didReceiveAuthenticationChallenge(handle, challenge);
        }
        void didCancelAuthenticationChallenge(ResourceHandle* handle, const AuthenticationChallenge& challenge)
        {
            if (m_handleClient)
                m_handleClient->didCancelAuthenticationChallenge(handle, challenge);
        }
#if USE(PROTECTION_SPACE_AUTH_CALLBACK)
        bool canAuthenticateAgainstProtectionSpace(ResourceHandle* handle, const ProtectionSpace& space)
        {
            return m_handleClient ? m_handleClient->canAuthenticateAgainstProtectionSpace(handle, space) : false;
        }
#endif
        void receivedCancellation(ResourceHandle* handle, const AuthenticationChallenge& challenge)
        {
            if (m_handleClient)
                m_handleClient->receivedCancellation(handle, challenge);
        }

#if PLATFORM(IOS) || USE(CFNETWORK)
        RetainPtr<CFDictionaryRef> connectionProperties(ResourceHandle* handle) { return m_handleClient ? m_handleClient->connectionProperties(handle) : nullptr; }
#endif

#if USE(CFNETWORK)
        CFCachedURLResponseRef willCacheResponse(ResourceHandle* handle, CFCachedURLResponseRef response)
        {
            return m_handleClient ? m_handleClient->willCacheResponse(handle, response) : nullptr;
        }
#if PLATFORM(WIN)
        bool shouldCacheResponse(ResourceHandle* handle, CFCachedURLResponseRef ref)
        {
            return m_handleClient ? m_handleClient->shouldCacheResponse(handle, ref) : true;
        }
#endif // PLATFORM(WIN)

#elif PLATFORM(COCOA)
        NSCachedURLResponse* willCacheResponse(ResourceHandle* handle, NSCachedURLResponse* response)
        {
            return m_handleClient ? m_handleClient->willCacheResponse(handle, response) : nullptr;
        }
#endif

#if USE(QUICK_LOOK)
        void didCreateQuickLookHandle(QuickLookHandle& handle)
        {
            if (m_handleClient)
                m_handleClient->didCreateQuickLookHandle(handle);
        }
#endif

    // ResourceResolver API
    void cancel()
    {
        if (m_handle) {
            m_handle->cancel();
            m_handle = nullptr;
        }
    }
    void setDefersLoading(bool defers)
    {
        if (m_handle)
            m_handle->setDefersLoading(defers);
    }

    ResourceResolverAsync* async() { return this; }
    ResourceHandle* handle() { return m_handle.get(); }

    // ResourceResolverAsync API
    void continueWillSendRequest(const ResourceRequest& request)
    {
        if (m_handle)
            m_handle->continueWillSendRequest(request);
    }
    void continueDidReceiveResponse()
    {
        if (m_handle)
            m_handle->continueDidReceiveResponse();
    }

    void clearClient()
    {
        m_client = nullptr;
        m_asyncClient = nullptr;
        m_handleClient = nullptr;
        if (m_handle)
            m_handle->setClient(nullptr);
    }

    bool isClient(ResourceResolverClient* client) const { return client == m_client; }
    ResourceRequest& firstRequest() { return m_firstRequest; }

    static PassRefPtr<ResourceResolver> create(NetworkingContext* context, const ResourceRequest& request, ResourceResolverClient* client, ResourceResolverAsyncClient* asyncClient, ResourceHandleClient* handleClient, bool defersLoading, bool shouldContentSniff)
    {
        RefPtr<ResourceHandleResolver> resolver(adoptRef(new ResourceHandleResolver(context, request, client, asyncClient, handleClient, defersLoading, shouldContentSniff)));
        if (!resolver)
            return nullptr;
        return resolver.release();
    }

private:
    ResourceHandleResolver(NetworkingContext* context, const ResourceRequest& request, ResourceResolverClient* client, ResourceResolverAsyncClient* asyncClient, ResourceHandleClient* handleClient, bool defersLoading, bool shouldContentSniff)
        : m_client(client)
        , m_asyncClient(asyncClient)
        , m_handleClient(handleClient)
        , m_firstRequest(request)
    {
        m_handle = ResourceHandle::create(context, request, client, asyncClient	, this, defersLoading, shouldContentSniff);
    }

    ResourceResolverClient* m_client;
    ResourceResolverAsyncClient* m_asyncClient;
    ResourceHandleClient* m_handleClient;
    RefPtr<ResourceHandle> m_handle;
    ResourceRequest m_firstRequest;

};

PassRefPtr<ResourceResolver> ResourceResolver::create(NetworkingContext* context, const ResourceRequest& request, ResourceResolverClient* client, ResourceResolverAsyncClient* asyncClient, ResourceHandleClient* handleClient, bool defersLoading, bool shouldContentSniff)
{
    if (request.url().protocolIs("blob")) {
        RefPtr<BlobResourceHandle> resolver = BlobResourceHandle::create(request, client, asyncClient);
        resolver->start();
        return resolver;
    }

    return ResourceHandleResolver::create(context, request, client, asyncClient, handleClient, defersLoading, shouldContentSniff);
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
