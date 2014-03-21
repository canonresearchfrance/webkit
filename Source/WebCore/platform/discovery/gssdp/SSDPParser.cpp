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
#include "SSDPParser.h"

#if ENABLE(DISCOVERY)

#include "Logging.h"
#include "NetworkServicesProviderBase.h"
#include "SSDPResolver.h"
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/xpathInternals.h>
#include <wtf/text/CString.h>

namespace WebCore {

SSDPParser::SSDPParser(SSDPResolver* client, const char* baseUri)
    : m_client(client)
    , m_baseUri(baseUri)
{
}

void SSDPParser::parse(const char * buffer, int size)
{
    xmlDocPtr doc;
    xmlNodePtr root;
    xmlChar* xpathDevice = (xmlChar*) "//ns:device";
    xmlChar* xpathUDN = (xmlChar*) "///ns:UDN";
    xmlXPathObjectPtr deviceList;
    xmlXPathObjectPtr serviceList;
    xmlXPathObjectPtr udnNodeList;
    xmlChar* udn = 0;
    xmlBufferPtr configBuf = 0;
    const xmlChar* ns = 0;

    xmlKeepBlanksDefault(0); 

    doc = xmlParseMemory(buffer, size);
    if (!doc)
        return;

    root = xmlDocGetRootElement(doc);
    if (root)
        ns = root->ns->href;
    
    deviceList = getNodeSet(doc, ns, xpathDevice);
    if (deviceList) {
        int i;
        xmlNodeSetPtr nodeSet = deviceList->nodesetval;

        udnNodeList = getNodeSet(doc, ns, xpathUDN);
        if (!udnNodeList) 
            return;

        /* Loop on <device> contains in the <deviceList> element */
        for (i = 0; i < nodeSet->nodeNr; i++) {
            char xpathService[32];
            xmlNodePtr udnNode = udnNodeList->nodesetval->nodeTab[i];
            xmlNodePtr deviceNode = xmlCopyNode(nodeSet->nodeTab[i], 1);

            udn = xmlNodeListGetString(doc, udnNode->xmlChildrenNode, 1);

            sprintf(xpathService, "//ns:device[%d]//ns:service", i+1);

            configBuf = xmlBufferCreate();
            if (!configBuf)
                break;

            xmlNodeDump(configBuf, doc, deviceNode, 1, 1);

            serviceList = getNodeSet(doc, ns, (xmlChar*)xpathService);
            if (serviceList) {
                /* Loop on <service> contains in the <serviceList> element */
                parseServiceList(doc, ns, udn, serviceList->nodesetval, configBuf);
                xmlXPathFreeObject(serviceList);
            }

            if (udn)
                xmlFree(udn);

            xmlBufferFree(configBuf);
            xmlFreeNode(deviceNode);            
        }

        xmlXPathFreeObject(udnNodeList);
        xmlXPathFreeObject(deviceList);
    }

    xmlFreeDoc(doc);
    xmlCleanupParser();
}

xmlXPathObjectPtr SSDPParser::getNodeSet(xmlDocPtr doc, const xmlChar *ns, xmlChar *xpath)
{
    xmlXPathContextPtr context;
    xmlXPathObjectPtr result = 0;
    
    context = xmlXPathNewContext(doc);
    if (context) {
        if (xmlXPathRegisterNs(context, (xmlChar*)"ns", ns))
            return 0;
        
        result = xmlXPathEvalExpression(xpath, context);
        xmlXPathFreeContext(context);
        
        if (result && xmlXPathNodeSetIsEmpty(result->nodesetval)) {
            xmlXPathFreeObject(result);
            return 0;
        }
    }
    return result;
}

void SSDPParser::parseServiceList(
    xmlDocPtr doc,
    const xmlChar* ns,
    const xmlChar* udn,
    xmlNodeSetPtr serviceNodeSet, 
    xmlBufferPtr configBuf)
{
    int i;

    for (i = 0; i < serviceNodeSet->nodeNr; i++) {
        xmlNodePtr serviceNode = serviceNodeSet->nodeTab[i];
        xmlNodePtr childNode = serviceNode->xmlChildrenNode; 
        xmlNodePtr next = childNode;
        char* serviceType = 0;
        char* serviceId = 0;
        char* controlURL = 0;
        char* eventSubURL = 0;

        /* check namespace */
        if (strcmp((const char*)serviceNode->ns->href, (const char*)ns))
            continue;

        while (next) {
            char* value = (char*)xmlNodeListGetString(doc, next->xmlChildrenNode, 1);
            bool found = false;

            if (!strcmp((const char*)next->name, "serviceType")) {
                serviceType = value;
                found = true;
            } else if (!strcmp((const char*)next->name, "serviceId")) {
                serviceId = value;
                found = true;
            } else if (!strcmp((const char*)next->name, "controlURL")) {
                controlURL = value;
                found = true;
            } else if (!strcmp((const char*)next->name, "eventSubURL")) {
                eventSubURL = value;
                found = true;
            }

            if (!found)
                xmlFree(value);

            next = xmlNextElementSibling(next);
        }
        
        if (udn && serviceId && serviceType && controlURL)
            addService((const char*)udn, serviceId, serviceType, controlURL, 
                (const char*)configBuf->content, eventSubURL);
        else
            LOG_ERROR("resolve failed for service id %s", serviceId);
        
        if (serviceType)
            xmlFree(serviceType);
        if (serviceId)
            xmlFree(serviceId);
        if (controlURL)
            xmlFree(controlURL);
        if (eventSubURL)
            xmlFree(eventSubURL);
    }
}

void SSDPParser::addService(
    const char* udn,
    const char* serviceId, 
    const char* serviceType, 
    const char* controlURL, 
    const char* config, 
    const char* eventSubURL) 
{
    char* id;
    char* type;
    char* url;
    char* eventUrl = 0;

    id = (char*)malloc(strlen(udn) + strlen(serviceId) + 3 /* "::" + '\0' */);
    if (!id)
        return;

    url = (char*)malloc(m_baseUri.length() + strlen(controlURL) + 1);
    if (!url) {
        free(id);
        return;
    } 

    if (eventSubURL) {
        char* eventUrl = (char*)malloc(m_baseUri.length() + strlen(eventSubURL) + 1);
        if (!eventUrl) {
            free(id);
            free(url);
        }
        sprintf(eventUrl, "%s%s", m_baseUri.utf8().data(), eventSubURL);
    }

    if (m_client->updateServiceDescription(id, url, config, eventUrl)) {
        free(id);
        free(url);
        if (eventUrl)
            free(eventUrl);
        return;
    }

    type = (char*)malloc(strlen(serviceType) + 6 /* "upnp:" + '\0' */);
    if (!type) {
        free(id);
        free(url);
        return;
    } 

    /* id     : The first occurrence of the <UDN> element in the device descriptor file 
     *          with the advertised service's <serviceId> sub-element. (udn::serviceId)
     * name   : The first occurrence of the advertised service's <serviceId> sub-element.
     * type   : The first occurrence of the advertised service's <serviceType> sub-element. (upnp:serviceType)
     * url    : The first occurrence of the advertised service's <controlURL> sub-element. (m_baseUri/controlURL)
     * config : The first occurrence of the <device> element in the device descriptor file. 
     */
    sprintf(id, "%s::%s", udn, serviceId);
    sprintf(type, "upnp:%s", serviceType);
    sprintf(url, "%s%s", m_baseUri.utf8().data(), controlURL);

    LOG(Network, "= %s\n"
        "   name = [%s]\n"
        "   type = [%s]\n"
        "   url = [%s]\n",
        id, serviceId, type, url);
    // LOG(Network, "   config = [%s]\n", config);

    m_client->addServiceDescription(id, (const char*)serviceId, type, url, config);

    if (eventUrl) {
        m_client->client()->setServiceEventSubURL(String(id), String(eventUrl));
        free(eventUrl);
    }

    free(id);
    free(type);
    free(url);
}

} // namespace WebCore

#endif // ENABLE(DISCOVERY)
