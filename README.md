WebKit with NSD API support
===========================

This branch of WebKit implements the W3C [Network Service Description](http://www.w3.org/TR/discovery-api/) (NSD) API.

Status
======
It supports most of http://www.w3.org/TR/2014/WD-discovery-api-20140220/.

* Support of UPnP and ZeroConf service discovery
* Support of the NSD events: onservicefound, onservicelost, onavailable, onunavailable, onnotify

It does not yet support:

* The Promise-like API
* DIAL service discovery

CORS support for detecting legacy devices is left out of this branch and
should be implemented within the application on top of this library.


This branch implements:

* Support of Abstract NSD API (within WebCore)
* Support of UPnP discovery through GUPnP and ZeroConf disocvery through Avahi
* Support of WebKit GTK and EFL ports
* WebKit1 and WebKit2 models

Organization
============

The titles of the NSD specific patches are prefixed by "NSD:".

Some folders containing most of the NSD implementation:

* Source/WebCore/Modules/discovery: implementation of the API and generic code
* Source/WebCore/platform/discovery: implementation of specific discovery providers (avahi, gupnp...)
* LayoutTests/discovery: tests specific to the NSD API
* UIProcess/Discovery and WebProcess/Discovery: WebKit2 support of the NSD API



Contributors:
* Ludovic Le Goff
* Youenn Fablet
