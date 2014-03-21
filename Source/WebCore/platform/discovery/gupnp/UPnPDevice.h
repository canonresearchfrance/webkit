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

#ifndef UPnPDevice_h
#define UPnPDevice_h

#if ENABLE(DISCOVERY)

#include <libgupnp/gupnp.h>
#include <libxml/xpath.h>
#include <wtf/RefCounted.h>
#include <wtf/text/WTFString.h>

namespace WebCore {

class UPnPDevice : public RefCounted<UPnPDevice> {
public:
    static PassRefPtr<UPnPDevice> create(GUPnPDeviceInfo* di) 
    { 
        return adoptRef(new UPnPDevice(di)); 
    }

    ~UPnPDevice();

    const String& udn() { return m_udn; }
    GUPnPDeviceInfo* info() { return m_deviceInfo; }
    const char* config();

private: 
    UPnPDevice(GUPnPDeviceInfo*);

    static xmlXPathObjectPtr getNodeSet(xmlDocPtr, const xmlChar*, xmlChar*);

    void parse(xmlDocPtr);

    GUPnPDeviceInfo* m_deviceInfo;
    String m_udn;
    xmlBufferPtr m_configBuf;
};

} // namespace WebCore

#endif // ENABLE(DISCOVERY)

#endif // UPnPDevice_h
