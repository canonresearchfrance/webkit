function dumpServices(services) {
    debug("* got reply: " + services.length + " service(s) on " + services.servicesAvailable + " services available");
    for (var i = 0; i < services.length; ++i) {
        debug("* services[" + i + "]: " + services[i].type);
        // check whether a service is present twice
        for (var j = 0; j < i-1; ++j) {
            if (services[i].id == services[j].id)
                debug("* ERROR services[" + i + "]: " + services[i].id + " present twice !");
                break;
        }
    }

}

// Callback function to be used for a successful network services call.
function onSuccessDefault(services) {
  debug("* got reply: " + services.length + " service(s)");

  var svcXhr = new XMLHttpRequest();
  svcXhr.open("GET", services[0].url, true);
  svcXhr.onreadystatechange = function() {
        if (svcXhr.readyState == 4) {
            if (svcXhr.status == 200 || svcXhr.status == 0) {
                debug("* response received");            
            } else {
                debug("* Xhr failed");       
            }

            if (window.testRunner) {
                window.testRunner.notifyDone();
            }
        }
    };
  svcXhr.send(null); 
  debug("* Xhr send to " + services[0].name);
}

// Callback function to be used for a failed network services call.
function onFailureDefault(error) {
  debug("* got failure: " + error.code);

  if (window.testRunner) {
    window.testRunner.notifyDone();
  }
}

// Launch a network services call with callbacks.
function getNetworkServicesWithCallbacks(types, successCB, errorCB) {

  if (successCB == null) {
      successCB = onSuccessDefault;
  }
  if (errorCB == null && arguments.length != 3) {
      debug("* register default error callback");
      errorCB = onFailureDefault;
  }

  debug("* sent network services request");
  if (errorCB == null) {
    navigator.getNetworkServices(types, successCB);
  } else {
    navigator.getNetworkServices(types, successCB, errorCB);
  }

  if (window.testRunner) {
    window.testRunner.waitUntilDone();
  } else {
    alert('This test needs to run in DRT');
  }
}

// This button press simulator sets the user gesture indicator that a network service
// provides access.
function simulateButtonPress() {
  var button = document.getElementById("button");
  if (eventSender) {
    eventSender.mouseMoveTo(button.getBoundingClientRect().left + 2, button.getBoundingClientRect().top + 12);
    eventSender.mouseDown();
    eventSender.mouseUp();
  }
}
