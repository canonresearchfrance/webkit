/*
 * Copyright (C) 2012-2014 Apple Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. AND ITS CONTRIBUTORS ``AS IS''
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL APPLE INC. OR ITS CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef NetworkResourceLoader_h
#define NetworkResourceLoader_h

#if ENABLE(NETWORK_PROCESS)

#include "MessageSender.h"
#include "NetworkConnectionToWebProcessMessages.h"
#include "NetworkResourceLoadParameters.h"
#include "ShareableResource.h"
#include <WebCore/ResourceError.h>
#include <WebCore/ResourceHandleClient.h>
#include <WebCore/ResourceLoaderOptions.h>
#include <WebCore/ResourceRequest.h>
#include <WebCore/ResourceResolverClient.h>
#include <WebCore/ResourceResolverAsync.h>
#include <WebCore/ResourceResponse.h>
#include <WebCore/SessionID.h>
#include <WebCore/Timer.h>
#include <wtf/MainThread.h>
#include <wtf/RunLoop.h>

typedef const struct _CFCachedURLResponse* CFCachedURLResponseRef;

namespace WebCore {
class BlobDataFileReference;
class ResourceHandle;
class ResourceRequest;
class ResourceResolver;
}

namespace WebKit {

class NetworkConnectionToWebProcess;
class NetworkResourceLoadParameters;
class RemoteNetworkingContext;
class SandboxExtension;

class NetworkResourceLoader : public RefCounted<NetworkResourceLoader>, public WebCore::ResourceHandleClient, public WebCore::ResourceResolverClient, public WebCore::ResourceResolverAsyncClient, public IPC::MessageSender {
public:
    static RefPtr<NetworkResourceLoader> create(const NetworkResourceLoadParameters& parameters, NetworkConnectionToWebProcess* connection)
    {
        return adoptRef(new NetworkResourceLoader(parameters, connection, nullptr));
    }
    
    static RefPtr<NetworkResourceLoader> create(const NetworkResourceLoadParameters& parameters, NetworkConnectionToWebProcess* connection, PassRefPtr<Messages::NetworkConnectionToWebProcess::PerformSynchronousLoad::DelayedReply> reply)
    {
        return adoptRef(new NetworkResourceLoader(parameters, connection, reply));
    }    
    ~NetworkResourceLoader();

    const WebCore::ResourceRequest& originalRequest() const { return m_parameters.request; }

    // Changes with redirects.
    WebCore::ResourceRequest& currentRequest() { return m_currentRequest; }

    WebCore::ResourceResolver* resolver() const { return m_resolver.get(); }
    void didConvertResolverToDownload();

    void start();
    void abort();

    void setDefersLoading(bool);

#if PLATFORM(COCOA)
    static size_t fileBackedResourceMinimumSize();
#endif
    // Message handlers.
    void didReceiveNetworkResourceLoaderMessage(IPC::Connection*, IPC::MessageDecoder&);

#if PLATFORM(IOS) || (PLATFORM(MAC) && __MAC_OS_X_VERSION_MIN_REQUIRED >= 1090)
    static void tryGetShareableHandleFromCFURLCachedResponse(ShareableResource::Handle&, CFCachedURLResponseRef);
    static void tryGetShareableHandleFromSharedBuffer(ShareableResource::Handle&, WebCore::SharedBuffer&);
#endif

#if USE(PROTECTION_SPACE_AUTH_CALLBACK)
    void continueCanAuthenticateAgainstProtectionSpace(bool);
#endif
    void continueWillSendRequest(const WebCore::ResourceRequest& newRequest);

    NetworkConnectionToWebProcess* connectionToWebProcess() const { return m_connection.get(); }
    WebCore::SessionID sessionID() const { return m_parameters.sessionID; }

    struct SynchronousLoadData;
    virtual bool usesAsyncCallbacks() override { return true; }

private:
    NetworkResourceLoader(const NetworkResourceLoadParameters&, NetworkConnectionToWebProcess*, PassRefPtr<Messages::NetworkConnectionToWebProcess::PerformSynchronousLoad::DelayedReply>);

    // IPC::MessageSender
    virtual IPC::Connection* messageSenderConnection() override;
    virtual uint64_t messageSenderDestinationID() override { return m_parameters.identifier; }

    // ResourceResolverClient
    virtual void didSendData(WebCore::ResourceResolver*, unsigned long long bytesSent, unsigned long long totalBytesToBeSent) override;
    virtual void didReceiveData(WebCore::ResourceResolver*, const char*, unsigned, int encodedDataLength) override;
    virtual void didReceiveBuffer(WebCore::ResourceResolver*, PassRefPtr<WebCore::SharedBuffer>, int encodedDataLength) override;
    virtual void didFinishLoading(WebCore::ResourceResolver*, double finishTime) override;
    virtual void didFail(WebCore::ResourceResolver*, const WebCore::ResourceError&) override;
    virtual void wasBlocked(WebCore::ResourceResolver*) override;
    virtual void cannotShowURL(WebCore::ResourceResolver*) override;
    virtual bool shouldUseCredentialStorage(WebCore::ResourceResolver*) override;

    // ResourceResolverAsyncClient
    virtual void willSendRequestAsync(WebCore::ResourceResolver*, const WebCore::ResourceRequest&, const WebCore::ResourceResponse& redirectResponse) override;
    virtual void didReceiveResponseAsync(WebCore::ResourceResolver*, const WebCore::ResourceResponse&) override;

    // FIXME: Remove these ResourceHandleClient declarations.
    using ResourceHandleClient::didSendData;
    using ResourceHandleClient::didReceiveData;
    using ResourceHandleClient::didReceiveBuffer;
    using ResourceHandleClient::didFinishLoading;
    using ResourceHandleClient::didFail;
    using ResourceHandleClient::wasBlocked;
    using ResourceHandleClient::cannotShowURL;
    using ResourceHandleClient::shouldUseCredentialStorage;

    using ResourceHandleClient::willSendRequestAsync;
    using ResourceHandleClient::didReceiveResponseAsync;

    // ResourceHandleClient
    virtual void didReceiveAuthenticationChallenge(WebCore::ResourceHandle*, const WebCore::AuthenticationChallenge&) override;
    virtual void didCancelAuthenticationChallenge(WebCore::ResourceHandle*, const WebCore::AuthenticationChallenge&) override;
    virtual void receivedCancellation(WebCore::ResourceHandle*, const WebCore::AuthenticationChallenge&) override;
#if USE(PROTECTION_SPACE_AUTH_CALLBACK)
    virtual void canAuthenticateAgainstProtectionSpaceAsync(WebCore::ResourceHandle*, const WebCore::ProtectionSpace&) override;
#endif
#if USE(NETWORK_CFDATA_ARRAY_CALLBACK)
    virtual bool supportsDataArray() override;
    virtual void didReceiveDataArray(WebCore::ResourceHandle*, CFArrayRef) override;
#endif
#if PLATFORM(COCOA)
#if USE(CFNETWORK)
    virtual void willCacheResponseAsync(WebCore::ResourceHandle*, CFCachedURLResponseRef) override;
#else
    virtual void willCacheResponseAsync(WebCore::ResourceHandle*, NSCachedURLResponse *) override;
#endif
#endif

    void continueDidReceiveResponse();

    void cleanup();
    
    void platformDidReceiveResponse(const WebCore::ResourceResponse&);

    void startBufferingTimerIfNeeded();
    void bufferingTimerFired();
    void sendBuffer(WebCore::SharedBuffer*, int encodedDataLength);

    bool isSynchronous() const;

    void consumeSandboxExtensions();
    void invalidateSandboxExtensions();

    template<typename T> bool sendAbortingOnFailure(T&& message, unsigned messageSendFlags = 0);

    const NetworkResourceLoadParameters m_parameters;

    RefPtr<NetworkConnectionToWebProcess> m_connection;

    RefPtr<RemoteNetworkingContext> m_networkingContext;
    RefPtr<WebCore::ResourceResolver> m_resolver;

    WebCore::ResourceRequest m_currentRequest;

    size_t m_bytesReceived;
    size_t m_bufferedDataEncodedDataLength;
    RefPtr<WebCore::SharedBuffer> m_bufferedData;

    std::unique_ptr<SynchronousLoadData> m_synchronousLoadData;
    Vector<RefPtr<WebCore::BlobDataFileReference>> m_fileReferences;

    bool m_didConvertResolverToDownload;
    bool m_didConsumeSandboxExtensions;
    bool m_defersLoading;

    WebCore::Timer m_bufferingTimer;
};

} // namespace WebKit

#endif // ENABLE(NETWORK_PROCESS)

#endif // NetworkResourceLoader_h
