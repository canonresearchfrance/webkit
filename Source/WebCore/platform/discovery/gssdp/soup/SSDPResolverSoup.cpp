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
#include "SSDPResolver.h"

#if ENABLE(DISCOVERY)

#include "NetworkServicesProviderGssdp.h"
#include <glib-object.h>
#include <string.h>
#include <wtf/PassRefPtr.h>

namespace WebCore {

SSDPResolver::SSDPResolver(NetworkServicesProviderBase* client)
    : SSDPResolverBase(client)
{
    m_soupSession = soup_session_async_new();
}

void SSDPResolver::resolve(const char* uri)
{
    SoupMessage* msg = soup_message_new("GET", uri);

    g_object_ref(msg);

    soup_session_queue_message(m_soupSession, msg, soupSessionCallback, static_cast<void*>(this));
}

void SSDPResolver::parseDescription()
{
    size_t i;
    SoupURI* soupUri;
    char* baseUri;
    char* pathAndQuery;
    char* ptr;

    for (i = 0; i < m_soupMessages.size(); i++) {
        SoupMessage* msg = m_soupMessages[i].get();

        /* retreive base uri */
        soupUri = soup_message_get_uri(msg);
        baseUri = soup_uri_to_string(soupUri, FALSE);
        pathAndQuery = soup_uri_to_string(soupUri, TRUE);
        if (strcmp(pathAndQuery, "/")) {
            ptr = strstr(baseUri, pathAndQuery);
            *ptr = '\0';
        } else {
            size_t len = strlen(baseUri) - 1;
            if (baseUri[len] == '/')
                baseUri[len] = '\0';
        }

        parse(baseUri, msg->response_body->data, msg->response_body->length);

        /* release soup uri strings */
        free(pathAndQuery);
        free(baseUri);

        // g_object_unref(msg);
    }

    m_soupMessages.clear();
}

void SSDPResolver::soupSessionCallback(SoupSession *session, SoupMessage *msg, gpointer userData)
{
    const gchar* header;
    SSDPResolver* resolver = static_cast<SSDPResolver*>(userData);

    if (SOUP_STATUS_IS_REDIRECTION(msg->status_code)) {
        header = soup_message_headers_get_one(msg->response_headers, "Location");
        if (header) {
            msg = soup_message_new("GET", header);
            soup_session_queue_message(session, msg, &soupSessionCallback, userData);
        }
        return;
    }
    
    if (!SOUP_STATUS_IS_SUCCESSFUL(msg->status_code))
        return;
    
    if (msg->response_body->length > 0) {
        resolver->m_soupMessages.append(adoptGRef(msg));
    
        if (!(static_cast<NetworkServicesProviderGssdp*>(resolver->client())->isActiveTimer()))
            resolver->parseDescription();
    }
}

} // namespace WebCore

#endif // ENABLE(DISCOVERY)
