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

#ifndef SSDPParser_h
#define SSDPParser_h

#if ENABLE(DISCOVERY)

#include <libxml/xpath.h>
#include <wtf/PassRefPtr.h>
#include <wtf/RefCounted.h>
#include <wtf/text/WTFString.h>

namespace WebCore {

class SSDPResolver;

class SSDPParser : public RefCounted<SSDPParser> {
public:
    static PassRefPtr<SSDPParser> create(SSDPResolver* client, const char* baseUri) 
    { 
        return adoptRef(new SSDPParser(client, baseUri)); 
    }

    void parse(const char*, int);

private: 
    SSDPParser(SSDPResolver*, const char*);

    static xmlXPathObjectPtr getNodeSet(xmlDocPtr, const xmlChar*, xmlChar*);
    void parseServiceList(xmlDocPtr, const xmlChar*, const xmlChar*, xmlNodeSetPtr, xmlBufferPtr);

    void addService(const char*, const char*, const char*, const char*, const char*, const char*);

    SSDPResolver* m_client;
    String m_baseUri;
};

} // namespace WebCore

#endif // ENABLE(DISCOVERY)

#endif // SSDPParser_h
