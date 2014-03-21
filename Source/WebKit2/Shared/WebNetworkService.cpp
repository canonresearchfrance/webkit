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
#include "WebNetworkService.h"

#include "ArgumentCoders.h"
#include "Arguments.h"
#include <wtf/text/CString.h>

namespace WebKit {

WebNetworkService::WebNetworkService(const String& id, const String& name, const String& type, const String& url, const String& config, bool online, bool corsEnable)
    : m_id(id)
    , m_name(name)
    , m_type(type)
    , m_url(url)
    , m_config(config)
    , m_online(online)
    , m_corsEnable(corsEnable)
{
}

void WebNetworkService::encode(IPC::ArgumentEncoder& encoder) const
{
    encoder << m_id;
    encoder << m_name;
    encoder << m_type;
    encoder << m_url;
    encoder << m_config;
    encoder << m_online;
    encoder << m_corsEnable;
}

bool WebNetworkService::decode(IPC::ArgumentDecoder& decoder, WebNetworkService& result)
{
    if (!decoder.decode(result.m_id))
        return false;
    if (!decoder.decode(result.m_name))
        return false;
    if (!decoder.decode(result.m_type))
        return false;
    if (!decoder.decode(result.m_url))
        return false;
    if (!decoder.decode(result.m_config))
        return false;
    if (!decoder.decode(result.m_online))
        return false;
    if (!decoder.decode(result.m_corsEnable))
        return false;

    return true;
}

} // namespace WebKit
