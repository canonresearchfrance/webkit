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

#ifndef SSDPResolver_h
#define SSDPResolver_h

#if ENABLE(DISCOVERY)

#include "SSDPResolverBase.h"
#include <libsoup/soup.h>
#include <wtf/PassRefPtr.h>
#include <wtf/RefCounted.h>
#include <wtf/Vector.h>
#include <wtf/gobject/GRefPtr.h>

namespace WebCore {

class NetworkServicesProviderBase;

class SSDPResolver : public RefCounted<SSDPResolver>, public SSDPResolverBase {
public:
    static PassRefPtr<SSDPResolver> create(NetworkServicesProviderBase* client) 
    {
        return adoptRef(new SSDPResolver(client));
    }

    ~SSDPResolver() { }
    void resolve(const char*);
    void parseDescription();

private: 
    SSDPResolver(NetworkServicesProviderBase*);

    static void soupSessionCallback(SoupSession*, SoupMessage*, gpointer);
    
    SoupSession* m_soupSession;

    Vector<GRefPtr<SoupMessage> > m_soupMessages;
};

} // namespace WebCore

#endif // ENABLE(DISCOVERY)

#endif // SSDPResolver_h
