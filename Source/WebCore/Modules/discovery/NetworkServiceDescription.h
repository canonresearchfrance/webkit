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

#ifndef NetworkServiceDescription_h
#define NetworkServiceDescription_h

#if ENABLE(DISCOVERY)

#include <wtf/text/WTFString.h>

namespace WebCore {

class NetworkServiceDescription {
public:
    NetworkServiceDescription(const char* id, const char* name, const char* type, const char* url, const char* config, const char* eventSubURL, bool corsEnable = false);
    NetworkServiceDescription(const String& id, const String& name, const String& type, const String& url, const String& config, const String& eventSubURL, bool corsEnable = false);
    
    const String& id() const { return m_id; }
    const String& name() const { return m_name; }
    const String& type() const { return m_type; }
    const String& url() const { return m_url; }
    const String& config() const { return m_config; }
    const String& eventSubURL() const { return m_eventSubURL; }
    bool online() const { return m_online; }
    bool corsEnable() const { return m_corsEnable; }

    void setOnline(bool online) { m_online = online; }
    void setEventSubURL(const String& eventSubURL) { m_eventSubURL = eventSubURL; }
    void update(const String& url, const String& config);
    
private:
    String m_id;
    String m_name;
    String m_type;
    String m_url;
    String m_config;
    String m_eventSubURL;
    bool m_online;
    bool m_corsEnable;
};

} // namespace WebCore

#endif // ENABLE(DISCOVERY)

#endif // NetworkServiceDescription_h
