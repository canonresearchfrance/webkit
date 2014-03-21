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
#include "UPnPDevice.h"

#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/xpathInternals.h>

#if ENABLE(DISCOVERY)

namespace WebCore {

UPnPDevice::UPnPDevice(GUPnPDeviceInfo* di)
    : m_deviceInfo(di)
    , m_udn(String(gupnp_device_info_get_udn(di)))
{
    GValue value = G_VALUE_INIT;
    
    g_value_init(&value, GUPNP_TYPE_XML_DOC);
    g_object_get_property(G_OBJECT(di), "document", &value);

    parse(((GUPnPXMLDoc*)g_value_get_object(&value))->doc);
}

UPnPDevice::~UPnPDevice()
{
    if (m_configBuf)
        xmlBufferFree(m_configBuf);
}

const char* UPnPDevice::config()
{
    if (m_configBuf)
        return (const char*)(m_configBuf->content);

    return 0;
}

xmlXPathObjectPtr UPnPDevice::getNodeSet(xmlDocPtr doc, const xmlChar *ns, xmlChar *xpath)
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

void UPnPDevice::parse(xmlDocPtr doc)
{
    xmlNodePtr root;
    xmlXPathObjectPtr device;
    const xmlChar* ns = 0;

    root = xmlDocGetRootElement(doc);
    if (root)
        ns = root->ns->href;
    
    device = getNodeSet(doc, ns, (xmlChar*) "//ns:device");
    if (device) {
        xmlNodeSetPtr nodeSet = device->nodesetval;
        xmlNodePtr deviceNode = xmlCopyNode(nodeSet->nodeTab[0], 1);

        m_configBuf = xmlBufferCreate();
        if (m_configBuf)
            xmlNodeDump(m_configBuf, doc, deviceNode, 1, 1);
            
        xmlFreeNode(deviceNode);
    }

    xmlCleanupParser();
}

} // namespace WebCore

#endif // ENABLE(DISCOVERY)
