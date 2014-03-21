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
#include "WebNetworkServices.h"

#include "ArgumentCoders.h"
#include "Arguments.h"

namespace WebKit {

WebNetworkServices::WebNetworkServices()
{
    m_data.length = 0;
    m_data.availableServices = 0;
}

WebNetworkServices::WebNetworkServices(unsigned long length, unsigned long availableServices, const String& origin, const Vector<WebNetworkService>& services)
{
    m_data.length = length;
    m_data.availableServices = availableServices;
    m_data.origin = origin;
    m_data.services = services;
}

void WebNetworkServices::Data::encode(IPC::ArgumentEncoder& encoder) const
{
    encoder << length;
    encoder << availableServices;
    encoder << origin;
    encoder << services;
}

bool WebNetworkServices::Data::decode(IPC::ArgumentDecoder& decoder, Data& result)
{
    if (!decoder.decode(result.length))
        return false;
    if (!decoder.decode(result.availableServices))
        return false;
    if (!decoder.decode(result.origin))
        return false;
    if (!decoder.decode(result.services))
        return false;

    return true;
}

} // namespace WebKit
