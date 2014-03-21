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

#include "UnitTestUtils/EWK2UnitTestBase.h"
#include "UnitTestUtils/EWK2UnitTestServer.h"

using namespace EWK2UnitTest;
using namespace WTF;

extern EWK2UnitTestEnvironment* environment;

struct NetworkServicesTestData {
    pid_t clientPID;
    Evas_Object *view;
    const char *indexHTML;
    const char *rule;
    Ewk_NetworkServices* networkServices;
};

static void check_network_service(Ewk_NetworkService* service, 
    const char* id,
    const char* name,
    const char* type,
    const char* url,
    const char* config,
    Eina_Bool online)
{
    const char* srvId = ewk_network_service_id_get(service);
    const char* srvName = ewk_network_service_name_get(service);
    const char* srvType = ewk_network_service_type_get(service);
    const char* srvUrl = ewk_network_service_url_get(service);
    const char* srvConfig = ewk_network_service_config_get(service);
    Eina_Bool srvOnline = ewk_network_service_is_online(service);

    ASSERT_STREQ(srvId, id);
    ASSERT_STREQ(srvName, name);
    ASSERT_STREQ(srvType, type);
    ASSERT_STREQ(srvUrl, url);
    ASSERT_STREQ(srvConfig, config);
    ASSERT_TRUE(srvOnline == online);
}

static void on_networkservices_request_started(void* userData, Evas_Object* webview, void* eventInfo)
{
    Ewk_NetworkServices* networkServices = (Ewk_NetworkServices*)eventInfo;
    NetworkServicesTestData* srvData = static_cast<NetworkServicesTestData*>(userData);

    ASSERT(networkServices);
    srvData->networkServices = networkServices;
}

static void on_networkservices_request_finished(void* userData, Evas_Object* webview, void* eventInfo)
{
    NetworkServicesTestData* srvData = static_cast<NetworkServicesTestData*>(userData);
    const char* origin;
    Ewk_NetworkService* service;
    char hostname[54];
    char url[68];
    char *ptr;

    gethostname(hostname, sizeof(hostname));
    ptr = strchr(hostname, '.');
    if(ptr) 
        *ptr = '\0';
    sprintf(url, "http://%s.local", hostname);

    ASSERT_EQ(1, ewk_network_services_length_get(srvData->networkServices));
    ASSERT_EQ(1, ewk_network_services_services_available_get(srvData->networkServices));

    origin = ewk_network_services_origin_get(srvData->networkServices);
    ASSERT_STREQ("file://", origin);

    service = ewk_network_services_item_get(srvData->networkServices, 0);

    ASSERT(service);

    check_network_service(service, 
                          "TestUnit._test-unit._tcp.local.", 
                          "TestUnit", 
                          "zeroconf:_test-unit._tcp", 
                          url,
                          "", 
                          EINA_TRUE);

    if (!strcmp(srvData->rule, "one_deny"))
        ewk_network_service_allowed_set(service, false);
    
    if (!strcmp(srvData->rule, "wait_update"))
    {
        kill(srvData->clientPID, SIGTERM);
        return;
    }

    if (!strcmp(srvData->rule, "all_deny"))
        ewk_network_services_denied_notify(srvData->networkServices);
    else
        ewk_network_services_allowed_notify(srvData->networkServices);

    ecore_main_loop_quit();
}

static void on_networkservices_request_updated(void* userData, Evas_Object* webview, void* eventInfo)
{
    Ewk_NetworkServices* networkServices = (Ewk_NetworkServices*)eventInfo;
    NetworkServicesTestData* srvData = static_cast<NetworkServicesTestData*>(userData);
    Ewk_NetworkService* service;
    char hostname[54];
    char url[68];
    char *ptr;

    gethostname(hostname, sizeof(hostname));
    ptr = strchr(hostname, '.');
    if(ptr) 
        *ptr = '\0';
    sprintf(url, "http://%s.local", hostname);

    ASSERT(networkServices);
    ASSERT_EQ(networkServices, srvData->networkServices);

    ASSERT(!strcmp(srvData->rule, "wait_update"));

    ASSERT_EQ(1, ewk_network_services_length_get(srvData->networkServices));

    service = ewk_network_services_item_get(srvData->networkServices, 0);

    ASSERT(service);

    check_network_service(service, 
                          "TestUnit._test-unit._tcp.local.", 
                          "TestUnit", 
                          "zeroconf:_test-unit._tcp", 
                          url,
                          "", 
                          EINA_FALSE);

    ecore_main_loop_quit();
}

static void on_networkservices_request_canceled(void* userData, Evas_Object* webview, void* eventInfo)
{
    Ewk_NetworkServices* networkServices = (Ewk_NetworkServices*)eventInfo;
    NetworkServicesTestData* srvData = static_cast<NetworkServicesTestData*>(userData);

    ASSERT(networkServices);
    ASSERT_EQ(networkServices, srvData->networkServices);

    ASSERT(!strcmp(srvData->rule, "wait_cancel"));
}

static void publishService()
{
    int ret = execlp("avahi-publish", "avahi-publish", 
                        "-s", "TestUnit", "_test-unit._tcp", "80", (char *) 0);
    
    if (ret == -1)
        fprintf(stderr, "Failed to publish service\n");
}

static void launchTest(NetworkServicesTestData *userData)
{
    pid_t pID = vfork();

    if (pID == 0) 
    {
        publishService(); 

        // Wait a while...
        sleep(2);

        _exit(0);
    } 
    else if (pID < 0) 
    {
        fprintf(stderr, "Failed to fork\n");
        exit(1);
    }
    else
    {
        userData->clientPID = pID;

        evas_object_smart_callback_add(userData->view, "networkservices,request,started", 
                                        on_networkservices_request_started, userData);
        evas_object_smart_callback_add(userData->view, "networkservices,request,finished", 
                                        on_networkservices_request_finished, userData);
        evas_object_smart_callback_add(userData->view, "networkservices,request,updated", 
                                        on_networkservices_request_updated, userData);
        evas_object_smart_callback_add(userData->view, "networkservices,request,canceled", 
                                        on_networkservices_request_canceled, userData);
        
        ewk_view_html_string_load(userData->view, userData->indexHTML, "file:///", 0);
    
        ecore_main_loop_begin();
    
        kill(pID, SIGTERM);
    }
}

TEST_F(EWK2UnitTestBase, ewk_network_services)
{
    const char indexHTML[] = "<html><body>Test Network Services Discovery API                   \
                                <script>                                                        \
                                    navigator.getNetworkServices(['zeroconf:_test-unit._tcp'],  \
                                        function onSuccess(services) {});                       \
                                </script></body></html>"; 
    NetworkServicesTestData userData;

    userData.view = webView();
    userData.indexHTML = indexHTML;
    userData.rule = "allow";

    launchTest(&userData);
}

TEST_F(EWK2UnitTestBase, ewk_network_services_one_deny)
{
    const char indexHTML[] = "<html><body>Test Network Services Discovery API                   \
                                <script>                                                        \
                                    navigator.getNetworkServices(['zeroconf:_test-unit._tcp'],  \
                                        function onSuccess(services) {},                        \
                                        function onError(error) { console.log(error)});         \
                                </script></body></html>"; 
    NetworkServicesTestData userData;

    userData.view = webView();
    userData.indexHTML = indexHTML;
    userData.rule = "one_deny";

    launchTest(&userData);
}

TEST_F(EWK2UnitTestBase, ewk_network_services_all_deny)
{
    const char indexHTML[] = "<html><body>Test Network Services Discovery API                   \
                                <script>                                                        \
                                    navigator.getNetworkServices(['zeroconf:_test-unit._tcp'],  \
                                        function onSuccess(services) {},                        \
                                        function onError(error) { console.log(error)});         \
                                </script></body></html>"; 
    NetworkServicesTestData userData;

    userData.view = webView();
    userData.indexHTML = indexHTML;
    userData.rule = "all_deny";

    launchTest(&userData);
}

TEST_F(EWK2UnitTestBase, ewk_network_services_cancel)
{
    const char indexHTML[] = "<html><body>Test Network Services Discovery API                               \
                                <script>                                                                    \
                                    function onSuccess(services) {}                                         \
                                    navigator.getNetworkServices(['zeroconf:_test-unit._tcp'], onSuccess);  \
                                    navigator.getNetworkServices(['zeroconf:_test-unit._tcp'], onSuccess);  \
                                </script></body></html>"; 
    NetworkServicesTestData userData;

    userData.view = webView();
    userData.indexHTML = indexHTML;
    userData.rule = "wait_cancel";

    launchTest(&userData);
}

TEST_F(EWK2UnitTestBase, ewk_network_services_update)
{
    const char indexHTML[] = "<html><body>Test Network Services Discovery API                               \
                                <script>                                                                    \
                                    function onSuccess(services) {}                                         \
                                    navigator.getNetworkServices(['zeroconf:_test-unit._tcp'], onSuccess);  \
                                </script></body></html>"; 
    NetworkServicesTestData userData;

    userData.view = webView();
    userData.indexHTML = indexHTML;
    userData.rule = "wait_update";

    launchTest(&userData);
}
