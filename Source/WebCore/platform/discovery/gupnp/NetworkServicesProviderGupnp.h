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

#ifndef NetworkServicesProviderGupnp_h
#define NetworkServicesProviderGupnp_h

#if ENABLE(DISCOVERY)

#include "NetworkServiceDescriptionGupnp.h"
#include "NetworkServicesProviderBase.h"
#include "Timer.h"
#include "UPnPDevice.h"
#include <libgupnp/gupnp.h>
#include <wtf/HashMap.h>
#include <wtf/PassRefPtr.h>
#include <wtf/RefPtr.h>
#include <wtf/Vector.h>
#include <wtf/gobject/GRefPtr.h>
#include <wtf/text/StringHash.h>
#include <wtf/text/WTFString.h>

namespace WebCore {

class NetworkServicesProviderClient;

class NetworkServicesProviderGupnp  : public NetworkServicesProviderBase {
public:
    NetworkServicesProviderGupnp(NetworkServicesProviderClient*);

    void startUpdating();
    void stopUpdating();

    void timerFired(Timer<NetworkServicesProviderGupnp>*);
    bool isActiveTimer() const { return m_timer.isActive(); }

    virtual void subscribeEventNotification(const String& id);

private: 
    static void upnpDeviceAvailable(GUPnPControlPoint*, GUPnPDeviceProxy*, gpointer);
    static void upnpDeviceUnavailable(GUPnPControlPoint*, GUPnPDeviceProxy*, gpointer);
    static void upnpServiceAvailable(GUPnPControlPoint*, GUPnPServiceProxy*, gpointer);
    static void upnpServiceUnavailable(GUPnPControlPoint*, GUPnPServiceProxy*, gpointer);
    static void upnpEventNotification(GUPnPServiceProxy*, const char*, GValue*, gpointer);
    
    static char* createId(GUPnPServiceInfo*);
    NetworkServiceDescriptionGupnp* getServiceDesc(GUPnPServiceInfo*, char* id = 0);

    void addPendingDescription(String&, NetworkServiceDescriptionGupnp*);
    void resolveAllPendingDesc();
    void resolvePendingDesc(PassRefPtr<UPnPDevice>);

    // GUPnP client
    GRefPtr<GUPnPContext> m_context;
    GRefPtr<GUPnPControlPoint> m_controlPoint;
    Timer<NetworkServicesProviderGupnp> m_timer;
    typedef HashMap<String, Vector<NetworkServiceDescriptionGupnp*> > PendingDesc;
    PendingDesc m_pendingDesc;
    HashMap<String, RefPtr<UPnPDevice> > m_devices;
};

} // namespace WebCore

#endif // ENABLE(DISCOVERY)

#endif // NetworkServicesProviderGupnp_h
