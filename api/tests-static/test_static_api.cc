/************************************************************************************************************************
 Copyright (c) 2016, Imagination Technologies Limited and/or its affiliated group companies.
 All rights reserved.

 Redistribution and use in source and binary forms, with or without modification, are permitted provided that the
 following conditions are met:
     1. Redistributions of source code must retain the above copyright notice, this list of conditions and the
        following disclaimer.
     2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the
        following disclaimer in the documentation and/or other materials provided with the distribution.
     3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote
        products derived from this software without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
 SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
 WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE 
 USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
************************************************************************************************************************/

#include <pthread.h>
#include <gtest/gtest.h>
#include "awa/static.h"
#include "awa/server.h"
#include "support/static_api_support.h"

namespace Awa {

class TestStaticClient : public testing::Test {};

TEST_F(TestStaticClient, AwaStaticClient_New_Free)
{
    AwaStaticClient * client = AwaStaticClient_New();
    ASSERT_TRUE(client != NULL);

    AwaStaticClient_Free(&client);
    ASSERT_TRUE(client == NULL);
}

TEST_F(TestStaticClient, AwaStaticClient_Init_NULL_Client)
{
    ASSERT_EQ(AwaError_StaticClientInvalid, AwaStaticClient_Init(NULL));
}

TEST_F(TestStaticClient, AwaStaticClient_New_Init_Unconfigured_Client)
{
    AwaStaticClient * client = AwaStaticClient_New();
    EXPECT_TRUE(client != NULL);

    ASSERT_EQ(AwaError_StaticClientNotConfigured, AwaStaticClient_Init(client));

    AwaStaticClient_Free(&client);
    EXPECT_TRUE(client == NULL);
}

TEST_F(TestStaticClient, AwaStaticClient_SetBootstrapServerURI_valid_input)
{
    AwaStaticClient * client = AwaStaticClient_New();
    EXPECT_TRUE(client != NULL);

    ASSERT_EQ(AwaError_Success, AwaStaticClient_SetBootstrapServerURI(client, "coap://127.0.0.1:15683/"));

    AwaStaticClient_Free(&client);
    EXPECT_TRUE(client == NULL);
}

TEST_F(TestStaticClient, AwaStaticClient_SetBootstrapServerURI_invalid_input)
{
    AwaStaticClient * client = AwaStaticClient_New();
    EXPECT_TRUE(client != NULL);

    ASSERT_EQ(AwaError_StaticClientInvalid, AwaStaticClient_SetBootstrapServerURI(NULL, "coap://127.0.0.1:15683/"));
    ASSERT_EQ(AwaError_StaticClientInvalid, AwaStaticClient_SetBootstrapServerURI(client, NULL));

    AwaStaticClient_Free(&client);
    EXPECT_TRUE(client == NULL);
}

TEST_F(TestStaticClient, AwaStaticClient_SetBootstrapServerURI_after_init)
{
    AwaStaticClient * client = AwaStaticClient_New();
    EXPECT_TRUE(client != NULL);

    EXPECT_EQ(AwaError_Success, AwaStaticClient_SetBootstrapServerURI(client, "coap://127.0.0.1:15683/"));
    EXPECT_EQ(AwaError_Success, AwaStaticClient_SetEndPointName(client, "imagination1"));
    EXPECT_EQ(AwaError_Success, AwaStaticClient_SetCoAPListenAddressPort(client, "0.0.0.0", 5683));

    EXPECT_EQ(AwaError_Success, AwaStaticClient_Init(client));

    EXPECT_EQ(AwaError_OperationInvalid, AwaStaticClient_SetBootstrapServerURI(client, "coap://127.0.0.2:15683/"));

    AwaStaticClient_Free(&client);
    EXPECT_TRUE(client == NULL);
}

TEST_F(TestStaticClient, AwaStaticClient_SetBootstrapServerURI_while_running)
{
    AwaStaticClient * client = AwaStaticClient_New();
    EXPECT_TRUE(client != NULL);

    EXPECT_EQ(AwaError_Success, AwaStaticClient_SetBootstrapServerURI(client, "coap://127.0.0.1:15683/"));
    EXPECT_EQ(AwaError_Success, AwaStaticClient_SetEndPointName(client, "imagination1"));
    EXPECT_EQ(AwaError_Success, AwaStaticClient_SetCoAPListenAddressPort(client, "0.0.0.0", 5683));

    EXPECT_EQ(AwaError_Success, AwaStaticClient_Init(client));

    AwaStaticClient_Process(client);

    EXPECT_EQ(AwaError_OperationInvalid, AwaStaticClient_SetBootstrapServerURI(client, "coap://127.0.0.2:15683/"));

    AwaStaticClient_Free(&client);
    EXPECT_TRUE(client == NULL);
}

TEST_F(TestStaticClient, AwaStaticClient_SetEndPointName_valid_input)
{
    AwaStaticClient * client = AwaStaticClient_New();
    EXPECT_TRUE(client != NULL);

    ASSERT_EQ(AwaError_Success, AwaStaticClient_SetEndPointName(client, "imagination1"));

    AwaStaticClient_Free(&client);
    EXPECT_TRUE(client == NULL);
}

TEST_F(TestStaticClient, AwaStaticClient_SetEndPointName_invalid_input)
{
    AwaStaticClient * client = AwaStaticClient_New();
    EXPECT_TRUE(client != NULL);

    ASSERT_EQ(AwaError_StaticClientInvalid, AwaStaticClient_SetEndPointName(NULL, "test"));
    ASSERT_EQ(AwaError_StaticClientInvalid, AwaStaticClient_SetEndPointName(client, NULL));

    AwaStaticClient_Free(&client);
    EXPECT_TRUE(client == NULL);
}

TEST_F(TestStaticClient, AwaStaticClient_SetEndPointName_long_name)
{
    AwaStaticClient * client = AwaStaticClient_New();
    EXPECT_TRUE(client != NULL);

    ASSERT_EQ(AwaError_OutOfMemory, AwaStaticClient_SetEndPointName(client, "123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890"));

    AwaStaticClient_Free(&client);
    EXPECT_TRUE(client == NULL);
}

TEST_F(TestStaticClient, AwaStaticClient_SetEndPointName_after_init)
{
    AwaStaticClient * client = AwaStaticClient_New();
    EXPECT_TRUE(client != NULL);

    EXPECT_EQ(AwaError_Success, AwaStaticClient_SetBootstrapServerURI(client, "coap://127.0.0.1:15683/"));
    EXPECT_EQ(AwaError_Success, AwaStaticClient_SetEndPointName(client, "imagination1"));
    EXPECT_EQ(AwaError_Success, AwaStaticClient_SetCoAPListenAddressPort(client, "0.0.0.0", 5683));

    EXPECT_EQ(AwaError_Success, AwaStaticClient_Init(client));

    EXPECT_EQ(AwaError_OperationInvalid, AwaStaticClient_SetEndPointName(client, "imagination12"));

    AwaStaticClient_Free(&client);
    EXPECT_TRUE(client == NULL);
}

TEST_F(TestStaticClient, AwaStaticClient_SetEndPointName_while_running)
{
    AwaStaticClient * client = AwaStaticClient_New();
    EXPECT_TRUE(client != NULL);

    EXPECT_EQ(AwaError_Success, AwaStaticClient_SetBootstrapServerURI(client, "coap://127.0.0.1:15683/"));
    EXPECT_EQ(AwaError_Success, AwaStaticClient_SetEndPointName(client, "imagination1"));
    EXPECT_EQ(AwaError_Success, AwaStaticClient_SetCoAPListenAddressPort(client, "0.0.0.0", 5683));

    EXPECT_EQ(AwaError_Success, AwaStaticClient_Init(client));

    AwaStaticClient_Process(client);

    EXPECT_EQ(AwaError_OperationInvalid, AwaStaticClient_SetEndPointName(client, "imagination12"));

    AwaStaticClient_Free(&client);
    EXPECT_TRUE(client == NULL);
}

TEST_F(TestStaticClient, AwaStaticClient_SetCoAPListenAddressPort_valid_input)
{
    AwaStaticClient * client = AwaStaticClient_New();
    EXPECT_TRUE(client != NULL);

    ASSERT_EQ(AwaError_Success, AwaStaticClient_SetCoAPListenAddressPort(client, "::", 5683));

    AwaStaticClient_Free(&client);
    EXPECT_TRUE(client == NULL);
}

TEST_F(TestStaticClient, AwaStaticClient_SetCoAPListenAddressPort_invalid_input)
{
    AwaStaticClient * client = AwaStaticClient_New();
    EXPECT_TRUE(client != NULL);

    ASSERT_EQ(AwaError_StaticClientInvalid, AwaStaticClient_SetCoAPListenAddressPort(NULL, "test", 5683));
    ASSERT_EQ(AwaError_StaticClientInvalid, AwaStaticClient_SetCoAPListenAddressPort(client, NULL, 5683));

    AwaStaticClient_Free(&client);
    EXPECT_TRUE(client == NULL);
}

TEST_F(TestStaticClient, AwaStaticClient_SetCoAPListenAddressPort_long_name)
{
    AwaStaticClient * client = AwaStaticClient_New();
    EXPECT_TRUE(client != NULL);

    ASSERT_EQ(AwaError_OutOfMemory, AwaStaticClient_SetCoAPListenAddressPort(client, "123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890", 5683));

    AwaStaticClient_Free(&client);
    EXPECT_TRUE(client == NULL);
}


TEST_F(TestStaticClient, AwaStaticClient_SetCoAPListenAddressPort_after_init)
{
    AwaStaticClient * client = AwaStaticClient_New();
    EXPECT_TRUE(client != NULL);

    EXPECT_EQ(AwaError_Success, AwaStaticClient_SetBootstrapServerURI(client, "coap://127.0.0.1:15683/"));
    EXPECT_EQ(AwaError_Success, AwaStaticClient_SetEndPointName(client, "imagination1"));
    EXPECT_EQ(AwaError_Success, AwaStaticClient_SetCoAPListenAddressPort(client, "0.0.0.0", 5683));

    EXPECT_EQ(AwaError_Success, AwaStaticClient_Init(client));

    ASSERT_EQ(AwaError_OperationInvalid, AwaStaticClient_SetCoAPListenAddressPort(client, "::", 5683));

    AwaStaticClient_Free(&client);
    EXPECT_TRUE(client == NULL);
}

TEST_F(TestStaticClient, AwaStaticClient_SetCoAPListenAddressPort_while_running)
{
    AwaStaticClient * client = AwaStaticClient_New();
    EXPECT_TRUE(client != NULL);

    EXPECT_EQ(AwaError_Success, AwaStaticClient_SetBootstrapServerURI(client, "coap://127.0.0.1:15683/"));
    EXPECT_EQ(AwaError_Success, AwaStaticClient_SetEndPointName(client, "imagination1"));
    EXPECT_EQ(AwaError_Success, AwaStaticClient_SetCoAPListenAddressPort(client, "0.0.0.0", 5683));

    EXPECT_EQ(AwaError_Success, AwaStaticClient_Init(client));

    AwaStaticClient_Process(client);

    ASSERT_EQ(AwaError_OperationInvalid, AwaStaticClient_SetCoAPListenAddressPort(client, "::", 5683));

    AwaStaticClient_Free(&client);
    EXPECT_TRUE(client == NULL);
}

TEST_F(TestStaticClient, AwaStaticClient_SetLogLevel_valid_input)
{
    DebugLevel oldLogLevel = Lwm2m_GetLogLevel();
    EXPECT_EQ(AwaError_Success, AwaStaticClient_SetLogLevel(AwaLogLevel_None));
    EXPECT_EQ(AwaError_Success, AwaStaticClient_SetLogLevel(AwaLogLevel_Debug));
    Lwm2m_SetLogLevel(oldLogLevel);
}

TEST_F(TestStaticClient, AwaStaticClient_SetLogLevel_invalid_input)
{
    DebugLevel oldLogLevel = Lwm2m_GetLogLevel();
    EXPECT_EQ(AwaError_LogLevelInvalid, AwaStaticClient_SetLogLevel(static_cast<AwaLogLevel>(AwaLogLevel_None - 1)));
    EXPECT_EQ(AwaError_LogLevelInvalid, AwaStaticClient_SetLogLevel(static_cast<AwaLogLevel>(AwaLogLevel_Debug + 1)));
    Lwm2m_SetLogLevel(oldLogLevel);
}


TEST_F(TestStaticClient, AwaStaticClient_SetApplicationContext_invalid_inputs)
{
    ASSERT_TRUE(NULL == AwaStaticClient_GetApplicationContext(NULL));
    ASSERT_EQ(AwaError_StaticClientInvalid, AwaStaticClient_SetApplicationContext(NULL, NULL));
}

TEST_F(TestStaticClient, AwaStaticClient_SetApplicationContext_valid_inputs)
{
    int dummycontext = 5;
    void * applicationContext = &dummycontext;
    AwaStaticClient * client = AwaStaticClient_New();
    EXPECT_TRUE(client != NULL);

    ASSERT_EQ(AwaError_Success, AwaStaticClient_SetApplicationContext(client, applicationContext));
    ASSERT_EQ(applicationContext, AwaStaticClient_GetApplicationContext(client));

    AwaStaticClient_Free(&client);
    EXPECT_TRUE(client == NULL);
}

TEST_F(TestStaticClient, AwaStaticClient_Init_not_configured)
{
    AwaStaticClient * client = AwaStaticClient_New();
    EXPECT_TRUE(client != NULL);

    ASSERT_EQ(AwaError_StaticClientNotConfigured, AwaStaticClient_Init(client));

    AwaStaticClient_Free(&client);
    EXPECT_TRUE(client == NULL);
}

TEST_F(TestStaticClient, AwaStaticClient_Init_valid_inputs)
{
    AwaStaticClient * client = AwaStaticClient_New();
    EXPECT_TRUE(client != NULL);

    EXPECT_EQ(AwaError_Success, AwaStaticClient_SetBootstrapServerURI(client, "coap://127.0.0.1:15683/"));
    EXPECT_EQ(AwaError_Success, AwaStaticClient_SetEndPointName(client, "imagination1"));
    EXPECT_EQ(AwaError_Success, AwaStaticClient_SetCoAPListenAddressPort(client, "0.0.0.0", 5683));

    ASSERT_EQ(AwaError_Success, AwaStaticClient_Init(client));

    AwaStaticClient_Free(&client);
    EXPECT_TRUE(client == NULL);
}

//This test is for valgrind
TEST_F(TestStaticClient, AwaStaticClient_Process)
{
    AwaStaticClient * client = AwaStaticClient_New();
    EXPECT_TRUE(client != NULL);

    EXPECT_EQ(AwaError_Success, AwaStaticClient_SetBootstrapServerURI(client, "coap://127.0.0.1:15683/"));
    EXPECT_EQ(AwaError_Success, AwaStaticClient_SetEndPointName(client, "imagination1"));
    EXPECT_EQ(AwaError_Success, AwaStaticClient_SetCoAPListenAddressPort(client, "0.0.0.0", 5683));

    EXPECT_EQ(AwaError_Success, AwaStaticClient_Init(client));

    AwaStaticClient_Process(client);
    AwaStaticClient_Process(client);
    AwaStaticClient_Process(client);
    AwaStaticClient_Process(client);

    AwaStaticClient_Free(&client);
    EXPECT_TRUE(client == NULL);
}

TEST_F(TestStaticClient, AwaStaticClient_Process_invalid_input)
{
    EXPECT_EQ(-1, AwaStaticClient_Process(NULL));
}

TEST_F(TestStaticClient, AwaStaticClinet_SetFactoryBootstrapInformation_before_init)
{
    AwaStaticClient * client = AwaStaticClient_New();
    EXPECT_TRUE(client != NULL);

    AwaFactoryBootstrapInfo bootstrapinfo = { 0 };

    sprintf(bootstrapinfo.SecurityInfo.ServerURI, "coap://127.0.0.1");
    bootstrapinfo.SecurityInfo.SecurityMode = AwaSecurityMode_NoSec;
    sprintf(bootstrapinfo.SecurityInfo.PublicKeyOrIdentity, "[PublicKey]");
    sprintf(bootstrapinfo.SecurityInfo.SecretKey, "[SecretKey]");

    bootstrapinfo.ServerInfo.Lifetime = 30;
    bootstrapinfo.ServerInfo.DefaultMinPeriod = 1;
    bootstrapinfo.ServerInfo.DefaultMaxPeriod = -1;
    bootstrapinfo.ServerInfo.DisableTimeout = 86400;
    bootstrapinfo.ServerInfo.Notification = false;
    sprintf(bootstrapinfo.ServerInfo.Binding, "U");

    ASSERT_EQ(AwaError_OperationInvalid, AwaStaticClient_SetFactoryBootstrapInformation(client, &bootstrapinfo));

    AwaStaticClient_Free(&client);
    EXPECT_TRUE(client == NULL);
}

TEST_F(TestStaticClient,  AwaStaticClient_SetFactoryBootstrapInformation_while_running)
{
    AwaStaticClient * client = AwaStaticClient_New();
    EXPECT_TRUE(client != NULL);

    EXPECT_EQ(AwaError_Success, AwaStaticClient_SetBootstrapServerURI(client, "coap://127.0.0.1:15683/"));
    EXPECT_EQ(AwaError_Success, AwaStaticClient_SetEndPointName(client, "imagination1"));
    EXPECT_EQ(AwaError_Success, AwaStaticClient_SetCoAPListenAddressPort(client, "0.0.0.0", 5683));

    EXPECT_EQ(AwaError_Success, AwaStaticClient_Init(client));

    AwaStaticClient_Process(client);

    AwaFactoryBootstrapInfo bootstrapinfo = { 0 };

    sprintf(bootstrapinfo.SecurityInfo.ServerURI, "coap://127.0.0.1");
    bootstrapinfo.SecurityInfo.SecurityMode = AwaSecurityMode_NoSec;
    sprintf(bootstrapinfo.SecurityInfo.PublicKeyOrIdentity, "[PublicKey]");
    sprintf(bootstrapinfo.SecurityInfo.SecretKey, "[SecretKey]");

    bootstrapinfo.ServerInfo.Lifetime = 30;
    bootstrapinfo.ServerInfo.DefaultMinPeriod = 1;
    bootstrapinfo.ServerInfo.DefaultMaxPeriod = -1;
    bootstrapinfo.ServerInfo.DisableTimeout = 86400;
    bootstrapinfo.ServerInfo.Notification = false;
    sprintf(bootstrapinfo.ServerInfo.Binding, "U");

    ASSERT_EQ(AwaError_OperationInvalid, AwaStaticClient_SetFactoryBootstrapInformation(client, &bootstrapinfo));

    AwaStaticClient_Free(&client);
    EXPECT_TRUE(client == NULL);
}

TEST_F(TestStaticClient,  AwaStaticClient_Bootstrap_Test)
{
    std::string testDescription = std::string(CURRENT_TEST_CASE_NAME + std::string(".") + CURRENT_TEST_NAME);

    int bootstrapServerCoapPort = 23667;
    std::string bootstrapServerAddress = "127.0.0.1";
    std::string bootstrapURI = std::string("coap://") + bootstrapServerAddress + std::string(":") + std::to_string(bootstrapServerCoapPort);
    AwaBootstrapServerDaemon bootstrapServerDaemon;
    bootstrapServerDaemon.SetCoapPort(bootstrapServerCoapPort);

    std::string serverAddress = "127.0.0.1";
    int serverCoapPort = 44443;
    int serverIpcPort = 6301;
    AwaServerDaemon serverDaemon;
    serverDaemon.SetCoapPort(serverCoapPort);
    serverDaemon.SetIpcPort(serverIpcPort);

#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
    std::string bootstrapConfigFilename = tmpnam(NULL);
#pragma GCC diagnostic pop
    BootstrapConfigFile bootstrapConfigFile (bootstrapConfigFilename, serverAddress, serverCoapPort);
    bootstrapServerDaemon.SetConfigFile(bootstrapConfigFile.GetFilename());

    // start the bootstrap and server daemons
    EXPECT_TRUE(bootstrapServerDaemon.Start(testDescription));
    EXPECT_TRUE(serverDaemon.Start(testDescription));

    std::string clientEndpointName = "BootstrapTestClient";
    AwaStaticClient * client = AwaStaticClient_New();
    EXPECT_TRUE(client != NULL);

    EXPECT_EQ(AwaError_Success, AwaStaticClient_SetBootstrapServerURI(client, bootstrapURI.c_str()));
    EXPECT_EQ(AwaError_Success, AwaStaticClient_SetEndPointName(client, clientEndpointName.c_str()));
    EXPECT_EQ(AwaError_Success, AwaStaticClient_SetCoAPListenAddressPort(client, "0.0.0.0", 5683));

    EXPECT_EQ(AwaError_Success, AwaStaticClient_Init(client));

    // wait for the client to register with the server
    AwaServerSession * session = AwaServerSession_New();
    EXPECT_TRUE(NULL != session);
    EXPECT_EQ(AwaError_Success, AwaServerSession_SetIPCAsUDP(session, serverAddress.c_str(), serverIpcPort));
    EXPECT_EQ(AwaError_Success, AwaServerSession_Connect(session));

    AwaServerListClientsOperation * operation = AwaServerListClientsOperation_New(session);
    EXPECT_TRUE(NULL != operation);

    SingleStaticClientPollCondition condition(client, operation, clientEndpointName, 10);
    ASSERT_TRUE(condition.Wait());

    AwaServerListClientsOperation_Free(&operation);
    AwaServerSession_Free(&session);

    AwaStaticClient_Free(&client);
    EXPECT_TRUE(client == NULL);

    serverDaemon.Stop();
    bootstrapServerDaemon.Stop();
}

TEST_F(TestStaticClientWithServer,  AwaStaticClient_Factory_Bootstrap_Test)
{
    AwaServerListClientsOperation * operation = AwaServerListClientsOperation_New(session_);
    EXPECT_TRUE(NULL != operation);

    SingleStaticClientPollCondition condition(client_, operation, global::clientEndpointName, 20);
    ASSERT_TRUE(condition.Wait());

    AwaServerListClientsOperation_Free(&operation);
}

TEST_F(TestStaticClientWithServer, AwaStaticClient_Create_and_Write_Operation_for_Object_and_Resource)
{
    struct callback1 : public StaticClientCallbackPollCondition
    {
        AwaInteger integer = 5;

        callback1(AwaStaticClient * StaticClient, int maxCount) : StaticClientCallbackPollCondition(StaticClient, maxCount) {};

        AwaResult handler(AwaStaticClient * context, AwaOperation operation, AwaObjectID objectID, AwaObjectInstanceID objectInstanceID, AwaResourceID resourceID, AwaResourceInstanceID resourceInstanceID, void ** dataPointer, size_t * dataSize, bool * changed)
        {

            AwaResult result = AwaResult_InternalError;
            EXPECT_TRUE((operation == AwaOperation_CreateResource) || (operation == AwaOperation_CreateObjectInstance) || (operation == AwaOperation_Write) || (operation == AwaOperation_Read));

            switch (operation)
            {
                case AwaOperation_CreateObjectInstance:
                {
                    EXPECT_EQ(9999, objectID);
                    EXPECT_EQ(0, objectInstanceID);
                    result = AwaResult_SuccessCreated;
                    break;
                }
                case AwaOperation_CreateResource:
                {
                    EXPECT_EQ(9999, objectID);
                    EXPECT_EQ(0, objectInstanceID);
                    EXPECT_EQ(1, resourceID);
                    result = AwaResult_SuccessCreated;
                    break;
                }
                case AwaOperation_Write:
                {
                    EXPECT_TRUE(dataPointer != NULL);
                    AwaInteger * integerPtr = (AwaInteger *)(*dataPointer);
                    EXPECT_TRUE(dataSize != NULL);
                    EXPECT_EQ(static_cast<size_t>(sizeof(AwaInteger)), *dataSize);
                    EXPECT_EQ(5, *integerPtr);
                    complete = true;
                    result = AwaResult_SuccessChanged;
                    break;
                }
                case AwaOperation_Read:
                {
                    EXPECT_TRUE(dataPointer != NULL);
                    EXPECT_TRUE(dataSize != NULL);
                    *dataPointer = &integer;
                    *dataSize = sizeof(integer);
                    result = AwaResult_SuccessContent;
                    break;
                }
                default:
                    result = AwaResult_InternalError;
                    break;
            }

            return result;
        }
    };

    callback1 cbHandler(client_, 20);
    EXPECT_EQ(AwaError_Success, AwaStaticClient_SetApplicationContext(client_, &cbHandler));
    EXPECT_EQ(AwaError_Success,AwaStaticClient_DefineObjectWithHandler(client_, "TestObject", 9999, 0, 1, handler));
    EXPECT_EQ(AwaError_Success, AwaStaticClient_DefineResourceWithHandler(client_, "TestResource", 9999, 1, AwaResourceType_Integer, 1, 1, AwaResourceOperations_ReadWrite, handler));

    AwaServerListClientsOperation * operation = AwaServerListClientsOperation_New(session_);
    EXPECT_TRUE(NULL != operation);
    SingleStaticClientPollCondition condition(client_, operation, global::clientEndpointName, 10);
    EXPECT_TRUE(condition.Wait());
    AwaServerListClientsOperation_Free(&operation);

    AwaServerDefineOperation * defineOperation = AwaServerDefineOperation_New(session_);
    EXPECT_TRUE(defineOperation != NULL);
    AwaObjectDefinition * objectDefinition = AwaObjectDefinition_New(9999, "TestObject", 0, 1);
    EXPECT_TRUE(objectDefinition != NULL);
    EXPECT_EQ(AwaError_Success, AwaObjectDefinition_AddResourceDefinitionAsInteger(objectDefinition, 1, "TestResource", true, AwaResourceOperations_ReadWrite, 0));
    EXPECT_EQ(AwaError_Success, AwaServerDefineOperation_Add(defineOperation, objectDefinition));
    EXPECT_EQ(AwaError_Success, AwaServerDefineOperation_Perform(defineOperation, defaults::timeout));
    AwaServerDefineOperation_Free(&defineOperation);
    AwaObjectDefinition_Free(&objectDefinition);

    AwaServerWriteOperation * writeOperation = AwaServerWriteOperation_New(session_, AwaWriteMode_Update);
    EXPECT_TRUE(writeOperation != NULL);
    EXPECT_EQ(AwaError_Success, AwaServerWriteOperation_CreateObjectInstance(writeOperation, "/9999/0"));
    EXPECT_EQ(AwaError_Success, AwaServerWriteOperation_AddValueAsInteger(writeOperation, "/9999/0/1", 5));

    pthread_t t;
    pthread_create(&t, NULL, do_write_operation, (void *)writeOperation);
    ASSERT_TRUE(cbHandler.Wait());
    pthread_join(t, NULL);

    AwaServerWriteOperation_Free(&writeOperation);
}

TEST_F(TestStaticClientWithServer, AwaStaticClient_Create_and_Read_Operation_for_Object_and_Resource)
{
    struct callback1 : public StaticClientCallbackPollCondition
    {
        AwaInteger integer = 5;
        int counter = 0;

        callback1(AwaStaticClient * StaticClient, int maxCount) : StaticClientCallbackPollCondition(StaticClient, maxCount) {};

        AwaResult handler(AwaStaticClient * context, AwaOperation operation, AwaObjectID objectID, AwaObjectInstanceID objectInstanceID, AwaResourceID resourceID, AwaResourceInstanceID resourceInstanceID, void ** dataPointer, size_t * dataSize, bool * changed)
        {
            AwaResult result = AwaResult_InternalError;
            EXPECT_TRUE((operation == AwaOperation_CreateResource) || (operation == AwaOperation_CreateObjectInstance) || (operation == AwaOperation_Read));

            switch(operation)
            {
                case AwaOperation_CreateObjectInstance:
                {
                    EXPECT_EQ(9999, objectID);
                    EXPECT_EQ(0, objectInstanceID);
                    result = AwaResult_SuccessCreated;
                    break;
                }
                case AwaOperation_CreateResource:
                {
                    EXPECT_EQ(9999, objectID);
                    EXPECT_EQ(0, objectInstanceID);
                    EXPECT_EQ(1, resourceID);
                    result = AwaResult_SuccessCreated;
                    break;
                }
                case AwaOperation_Read:
                {
                    EXPECT_TRUE(dataPointer != NULL);
                    EXPECT_TRUE(dataSize != NULL);
                    *dataPointer = &integer;
                    *dataSize = sizeof(integer);
                    complete = counter++ == 1 ? true : false;
                    result = AwaResult_SuccessContent;
                    break;
                }
                default:
                    result = AwaResult_InternalError;
                    break;
            }

            return result;
        }
    };

    callback1 cbHandler(client_, 20);

    EXPECT_EQ(AwaError_Success, AwaStaticClient_SetApplicationContext(client_, &cbHandler));
    EXPECT_EQ(AwaError_Success,AwaStaticClient_DefineObjectWithHandler(client_, "TestObject", 9999, 0, 1, handler));
    EXPECT_EQ(AwaError_Success, AwaStaticClient_DefineResourceWithHandler(client_, "TestResource", 9999, 1, AwaResourceType_Integer, 1, 1, AwaResourceOperations_ReadWrite, handler));
    EXPECT_EQ(AwaError_Success, AwaStaticClient_CreateObjectInstance(client_, 9999, 0));

    AwaServerListClientsOperation * operation = AwaServerListClientsOperation_New(session_);
    EXPECT_TRUE(NULL != operation);
    SingleStaticClientPollCondition condition(client_, operation, global::clientEndpointName, 10);
    EXPECT_TRUE(condition.Wait());
    AwaServerListClientsOperation_Free(&operation);

    AwaServerDefineOperation * defineOperation = AwaServerDefineOperation_New(session_);
    EXPECT_TRUE(defineOperation != NULL);
    AwaObjectDefinition * objectDefinition = AwaObjectDefinition_New(9999, "TestObject", 0, 1);
    EXPECT_TRUE(objectDefinition != NULL);
    EXPECT_EQ(AwaError_Success, AwaObjectDefinition_AddResourceDefinitionAsInteger(objectDefinition, 1, "TestResource", true, AwaResourceOperations_ReadWrite, 0));
    EXPECT_EQ(AwaError_Success, AwaServerDefineOperation_Add(defineOperation, objectDefinition));
    EXPECT_EQ(AwaError_Success, AwaServerDefineOperation_Perform(defineOperation, defaults::timeout));
    AwaServerDefineOperation_Free(&defineOperation);
    AwaObjectDefinition_Free(&objectDefinition);

    AwaServerReadOperation * readOperation = AwaServerReadOperation_New(session_);
    EXPECT_TRUE(readOperation != NULL);
    EXPECT_EQ(AwaError_Success, AwaServerReadOperation_AddPath(readOperation, global::clientEndpointName, "/9999/0"));

    pthread_t t;
    pthread_create(&t, NULL, do_read_operation, (void *)readOperation);
    ASSERT_TRUE(cbHandler.Wait());
    pthread_join(t, NULL);

    AwaServerReadOperation_Free(&readOperation);
}

void * do_delete_operation(void * attr)
{
    AwaServerDeleteOperation * deleteOperation = (AwaServerDeleteOperation *)attr;
    AwaServerDeleteOperation_Perform(deleteOperation, defaults::timeout);
    return 0;
}

TEST_F(TestStaticClientWithServer, AwaStaticClient_Create_and_Delete_Operation_for_Object_and_Resource)
{
    struct callback1 : public StaticClientCallbackPollCondition
    {
        AwaInteger integer = 5;
        callback1(AwaStaticClient * StaticClient, int maxCount) : StaticClientCallbackPollCondition(StaticClient, maxCount) {};

        AwaResult handler(AwaStaticClient * context, AwaOperation operation, AwaObjectID objectID, AwaObjectInstanceID objectInstanceID, AwaResourceID resourceID, AwaResourceInstanceID resourceInstanceID, void ** dataPointer, size_t * dataSize, bool * changed)

        {
            AwaResult result = AwaResult_InternalError;
            EXPECT_TRUE((operation == AwaOperation_CreateResource) || (operation == AwaOperation_CreateObjectInstance) || (operation == AwaOperation_DeleteObjectInstance) || (operation == AwaOperation_Read));

            switch(operation)
            {

                case AwaOperation_CreateObjectInstance:
                case AwaOperation_DeleteObjectInstance:
                {
                    EXPECT_EQ(9999, objectID);
                    EXPECT_EQ(0, objectInstanceID);
                    result = operation == AwaOperation_CreateObjectInstance ? AwaResult_SuccessCreated : AwaResult_SuccessDeleted;

                    if (operation == AwaOperation_DeleteObjectInstance)
                        complete = true;

                    break;
                }
                case AwaOperation_CreateResource:
                {
                    EXPECT_EQ(9999, objectID);
                    EXPECT_EQ(0, objectInstanceID);
                    EXPECT_EQ(1, resourceID);
                    result = AwaResult_SuccessCreated;
                    break;
                }
                case AwaOperation_Read:
                {
                    EXPECT_TRUE(dataPointer != NULL);
                    EXPECT_TRUE(dataSize != NULL);
                    *dataPointer = &integer;
                    *dataSize = sizeof(integer);
                    result = AwaResult_SuccessContent;
                    break;
                }
                default:
                    result = AwaResult_InternalError;
                    break;
            }

            return result;
        }
    };

    callback1 cbHandler(client_, 20);
    EXPECT_EQ(AwaError_Success, AwaStaticClient_SetApplicationContext(client_, &cbHandler));
    EXPECT_EQ(AwaError_Success, AwaStaticClient_DefineObjectWithHandler(client_, "TestObject", 9999, 0, 1, handler));
    EXPECT_EQ(AwaError_Success, AwaStaticClient_DefineResourceWithHandler(client_, "TestResource", 9999, 1, AwaResourceType_Integer, 0, 1, AwaResourceOperations_ReadWrite, handler));
    EXPECT_EQ(AwaError_Success, AwaStaticClient_CreateObjectInstance(client_, 9999, 0));
    EXPECT_EQ(AwaError_Success, AwaStaticClient_CreateResource(client_, 9999, 0, 1));

    AwaServerListClientsOperation * operation = AwaServerListClientsOperation_New(session_);
    EXPECT_TRUE(NULL != operation);
    SingleStaticClientPollCondition condition(client_, operation, global::clientEndpointName, 10);
    EXPECT_TRUE(condition.Wait());
    AwaServerListClientsOperation_Free(&operation);

    AwaServerDefineOperation * defineOperation = AwaServerDefineOperation_New(session_);
    EXPECT_TRUE(defineOperation != NULL);
    AwaObjectDefinition * objectDefinition = AwaObjectDefinition_New(9999, "TestObject", 0, 1);
    EXPECT_TRUE(objectDefinition != NULL);
    EXPECT_EQ(AwaError_Success, AwaObjectDefinition_AddResourceDefinitionAsInteger(objectDefinition, 1, "TestResource", false, AwaResourceOperations_ReadWrite, 0));
    EXPECT_EQ(AwaError_Success, AwaServerDefineOperation_Add(defineOperation, objectDefinition));
    EXPECT_EQ(AwaError_Success, AwaServerDefineOperation_Perform(defineOperation, defaults::timeout));
    AwaServerDefineOperation_Free(&defineOperation);
    AwaObjectDefinition_Free(&objectDefinition);

    AwaServerDeleteOperation * deleteOperation = AwaServerDeleteOperation_New(session_);
    EXPECT_TRUE(deleteOperation != NULL);
    EXPECT_EQ(AwaError_Success, AwaServerDeleteOperation_AddPath(deleteOperation, global::clientEndpointName, "/9999/0"));

    pthread_t t;
    pthread_create(&t, NULL, do_delete_operation, (void *)deleteOperation);
    ASSERT_TRUE(cbHandler.Wait());
    pthread_join(t, NULL);

    AwaServerDeleteOperation_Free(&deleteOperation);
}

void * do_execute_operation(void * attr)
{
    AwaServerExecuteOperation * executeOperation = (AwaServerExecuteOperation *)attr;
    AwaError result = AwaServerExecuteOperation_Perform(executeOperation, defaults::timeout);
    Lwm2m_Debug("AwaServerExecuteOperation_Perform: %s\n", AwaError_ToString(result));
    return 0;
}

TEST_F(TestStaticClientWithServer, AwaStaticClient_Create_and_Execute_Operation_for_Object_and_Resource)
{
    struct callback1 : public StaticClientCallbackPollCondition
    {
        callback1(AwaStaticClient * StaticClient, int maxCount) : StaticClientCallbackPollCondition(StaticClient, maxCount) {};

        AwaResult handler(AwaStaticClient * context, AwaOperation operation, AwaObjectID objectID, AwaObjectInstanceID objectInstanceID, AwaResourceID resourceID, AwaResourceInstanceID resourceInstanceID, void ** dataPointer, size_t * dataSize, bool * changed)
        {
            AwaResult result = AwaResult_InternalError;

            EXPECT_TRUE((operation == AwaOperation_CreateResource) || (operation == AwaOperation_CreateObjectInstance) || (operation == AwaOperation_Execute));

            switch(operation)
            {
                case AwaOperation_CreateObjectInstance:
                case AwaOperation_DeleteObjectInstance:
                {
                    EXPECT_EQ(9999, objectID);
                    EXPECT_EQ(0, objectInstanceID);
                    result = operation == AwaOperation_CreateObjectInstance ? AwaResult_SuccessCreated : AwaResult_SuccessDeleted;
                    break;
                }
                case AwaOperation_CreateResource:
                {
                    EXPECT_EQ(9999, objectID);
                    EXPECT_EQ(0, objectInstanceID);
                    EXPECT_EQ(1, resourceID);
                    result = AwaResult_SuccessCreated;
                    break;
                }
                case AwaOperation_Execute:
                {
                    EXPECT_EQ(9999, objectID);
                    EXPECT_EQ(0, objectInstanceID);
                    EXPECT_EQ(1, resourceID);
                    result = AwaResult_Success;
                    complete = true;
                    break;
                }
                default:
                    result = AwaResult_InternalError;
                    break;
            }

            return result;
        }
    };

    callback1 cbHandler(client_, 20);
    EXPECT_EQ(AwaError_Success, AwaStaticClient_SetApplicationContext(client_, &cbHandler));
    EXPECT_EQ(AwaError_Success, AwaStaticClient_DefineObjectWithHandler(client_, "TestObject", 9999, 0, 1, handler));
    EXPECT_EQ(AwaError_Success, AwaStaticClient_DefineResourceWithHandler(client_, "TestResource", 9999, 1, AwaResourceType_None, 1, 1, AwaResourceOperations_Execute, handler));
    EXPECT_EQ(AwaError_Success, AwaStaticClient_CreateObjectInstance(client_, 9999, 0));

    AwaServerListClientsOperation * operation = AwaServerListClientsOperation_New(session_);
    EXPECT_TRUE(NULL != operation);
    SingleStaticClientPollCondition condition(client_, operation, global::clientEndpointName, 10);
    EXPECT_TRUE(condition.Wait());
    AwaServerListClientsOperation_Free(&operation);

    AwaServerDefineOperation * defineOperation = AwaServerDefineOperation_New(session_);
    EXPECT_TRUE(defineOperation != NULL);
    AwaObjectDefinition * objectDefinition = AwaObjectDefinition_New(9999, "TestObject", 0, 1);
    EXPECT_TRUE(objectDefinition != NULL);
    EXPECT_EQ(AwaError_Success, AwaObjectDefinition_AddResourceDefinitionAsNoType(objectDefinition, 1, "TestExecutableResource", true, AwaResourceOperations_Execute));
    EXPECT_EQ(AwaError_Success, AwaServerDefineOperation_Add(defineOperation, objectDefinition));
    EXPECT_EQ(AwaError_Success, AwaServerDefineOperation_Perform(defineOperation, defaults::timeout));
    AwaServerDefineOperation_Free(&defineOperation);
    AwaObjectDefinition_Free(&objectDefinition);

    AwaServerExecuteOperation * executeOperation = AwaServerExecuteOperation_New(session_);
    EXPECT_TRUE(executeOperation != NULL);
    EXPECT_EQ(AwaError_Success, AwaServerExecuteOperation_AddPath(executeOperation, global::clientEndpointName, "/9999/0/1", NULL));

    pthread_t t;
    pthread_create(&t, NULL, do_execute_operation, (void *)executeOperation);
    ASSERT_TRUE(cbHandler.Wait());
    pthread_join(t, NULL);

    AwaServerExecuteOperation_Free(&executeOperation);
}


namespace writeDetail
{

struct TestWriteResource
{
    AwaError ExpectedAddResult;
    AwaError ExpectedProcessResult;

    AwaObjectID ObjectID;
    AwaObjectInstanceID ObjectInstanceID;
    AwaResourceID ResourceID;

    const void * Value;
    const size_t ValueCount;
    AwaResourceType Type;

    bool UseOperation;
};

::std::ostream& operator<<(::std::ostream& os, const TestWriteResource& item)
{
  return os << "Item: ExpectedAddResult " << item.ExpectedAddResult
            << ", ExpectedProcessResult " << item.ExpectedProcessResult
            << ", ObjectID " << item.ObjectID
            << ", ObjectInstanceID " << item.ObjectInstanceID
            << ", ResourceID " << item.ResourceID
            << ", Value " << item.Value
            << ", ValueCount " << item.ValueCount
            << ", Type " << item.Type
            << ", UseOperation " << item.UseOperation;
}

const char * clientID = "TestClient1";

static AwaInteger dummyInteger1 = 123456;
static const char * dummyString1 = "Lightweight M2M Server";
static AwaFloat dummyFloat1 = 1.0;
static AwaTime dummyTime1 = 0xA20AD72B;
static AwaBoolean dummyBoolean1 = true;
static char dummyOpaqueData[] = {'a',0,'x','\0', 123};
//static int dummyObjLinkData[] = {-1,-1};

AwaObjectLink dummyObjectLink1 = { 3, 5 };

const char * dummyStringArray1[] = {"Lightweight M2M Server", "test1", ""};
const char * dummyStringArray2[] = {"Lightweight M2M Server", "test1", "", "", "", ""};
const AwaInteger dummyIntegerArray1[] = {55, 8732, 11};
const AwaInteger dummyIntegerArray2[] = {55, 8732, 11, 55, 8732, 11};
const AwaFloat dummyFloatArray1[] = {55.0, 0.0008732, 11e10};
const AwaFloat dummyFloatArray2[] = {55.0, 0.0008732, 11e10, 55.0, 0.0008732, 11e10};
const AwaBoolean dummyBooleanArray1[] = {true, false, true};
const AwaBoolean dummyBooleanArray2[] = {true, false, true, true, false, true};

const AwaInteger dummyTimeArray1[] = {16000, 8732222, 1111};
const AwaInteger dummyTimeArray2[] = {16000, 8732222, 1111, 16000, 8732222, 1111};

const AwaObjectID TEST_OBJECT_NON_ARRAY_TYPES = 10000;
const AwaResourceID TEST_RESOURCE_STRING = 1;
const AwaResourceID TEST_RESOURCE_INTEGER = 2;
const AwaResourceID TEST_RESOURCE_FLOAT = 3;
const AwaResourceID TEST_RESOURCE_BOOLEAN = 4;
const AwaResourceID TEST_RESOURCE_OPAQUE = 5;
const AwaResourceID TEST_RESOURCE_TIME = 6;
const AwaResourceID TEST_RESOURCE_OBJECTLINK = 7;

const AwaObjectID TEST_OBJECT_ARRAY_TYPES = 10001;
const AwaResourceID TEST_RESOURCE_STRINGARRAY = 1;
const AwaResourceID TEST_RESOURCE_INTEGERARRAY = 2;
const AwaResourceID TEST_RESOURCE_FLOATARRAY = 3;
const AwaResourceID TEST_RESOURCE_BOOLEANARRAY = 4;
const AwaResourceID TEST_RESOURCE_OPAQUEARRAY = 5;
const AwaResourceID TEST_RESOURCE_TIMEARRAY = 6;
const AwaResourceID TEST_RESOURCE_OBJECTLINKARRAY = 7;

}

typedef AwaResult (*TestHandler)(void * context, AwaOperation operation, AwaObjectID objectID, AwaObjectInstanceID objectInstanceID, AwaResourceID resourceID, AwaResourceInstanceID resourceInstanceID, void ** dataPointer, size_t * dataSize, bool * changed);

struct TestWriteReadStaticResource
{
    TestHandler WriteHandler;
    TestHandler ReadHandler;

    AwaObjectID ObjectID;
    AwaObjectInstanceID ObjectInstanceID;
    AwaResourceID ResourceID;

    const void * Value;
    const int ValueCount;
    const int ValueSize;
    AwaResourceType Type;

    bool Complete;
    bool TestRead;
};

::std::ostream& operator<<(::std::ostream& os, const TestWriteReadStaticResource& item)
{
  return os << "Item: WriteHandler " << item.WriteHandler
            << ", ReadHandler " << item.ReadHandler
            << ", ObjectID " << item.ObjectID
            << ", ObjectInstanceID " << item.ObjectInstanceID
            << ", ResourceID " << item.ResourceID
            << ", Value " << item.Value
            << ", ValueCount " << item.ValueCount
            << ", ValueSize " << item.ValueSize
            << ", Type " << item.Type
            << ", Complete " << item.Complete
            << ", TestRead " << item.TestRead;
}

class TestStaticClientWriteReadValue : public TestStaticClientWithServer, public ::testing::WithParamInterface< TestWriteReadStaticResource >
{

protected:

    class callback1 : public StaticClientCallbackPollCondition
    {
    public:

        TestWriteReadStaticResource data;

        callback1(AwaStaticClient * StaticClient, int maxCount, TestWriteReadStaticResource data) : StaticClientCallbackPollCondition(StaticClient, maxCount), data(data) {};

        AwaResult handler(AwaStaticClient * context, AwaOperation operation, AwaObjectID objectID, AwaObjectInstanceID objectInstanceID, AwaResourceID resourceID, AwaResourceInstanceID resourceInstanceID, void ** dataPointer, size_t * dataSize, bool * changed)
        {
            AwaResult result = AwaResult_InternalError;

            switch (operation)
            {
                    case AwaOperation_CreateObjectInstance:
                    {
                        EXPECT_EQ(data.ObjectID, objectID);
                        EXPECT_EQ(data.ObjectInstanceID, objectInstanceID);
                        result = AwaResult_SuccessCreated;
                        break;
                    }
                    case AwaOperation_CreateResource:
                    {
                        EXPECT_EQ(data.ObjectID, objectID);
                        EXPECT_EQ(data.ObjectInstanceID, objectInstanceID);
                        EXPECT_EQ(data.ResourceID, resourceID);
                        result = AwaResult_SuccessCreated;
                        break;
                    }
                    case AwaOperation_Write:
                        result = data.WriteHandler(&data, operation, objectID, objectInstanceID, resourceID, resourceInstanceID, dataPointer, dataSize, changed);
                        break;
                    case AwaOperation_Read:
                        result = data.ReadHandler(&data, operation, objectID, objectInstanceID, resourceID, resourceInstanceID, dataPointer, dataSize, changed);
                        break;
                    default:
                        break;
            }


            complete = data.Complete;
            return result;
        }
    };

    callback1 * cbHandler;


    void SetUp() {
        TestStaticClientWithServer::SetUp();
        TestWriteReadStaticResource data = GetParam();

        cbHandler = new callback1(client_, 20, data);

        EXPECT_EQ(AwaError_Success, AwaStaticClient_SetApplicationContext(client_, cbHandler));
        EXPECT_EQ(AwaError_Success,AwaStaticClient_DefineObjectWithHandler(client_, "Test Object Single", writeDetail::TEST_OBJECT_NON_ARRAY_TYPES, 0, 1, handler));
        EXPECT_EQ(AwaError_Success, AwaStaticClient_DefineResourceWithHandler(client_, "Test String Resource",      writeDetail::TEST_OBJECT_NON_ARRAY_TYPES, writeDetail::TEST_RESOURCE_STRING,     AwaResourceType_String,     0, 1, AwaResourceOperations_ReadWrite, handler));
        EXPECT_EQ(AwaError_Success, AwaStaticClient_DefineResourceWithHandler(client_, "Test Integer Resource",     writeDetail::TEST_OBJECT_NON_ARRAY_TYPES, writeDetail::TEST_RESOURCE_INTEGER,    AwaResourceType_Integer,    0, 1, AwaResourceOperations_ReadWrite, handler));
        EXPECT_EQ(AwaError_Success, AwaStaticClient_DefineResourceWithHandler(client_, "Test Float Resource",       writeDetail::TEST_OBJECT_NON_ARRAY_TYPES, writeDetail::TEST_RESOURCE_FLOAT,      AwaResourceType_Float,      0, 1, AwaResourceOperations_ReadWrite, handler));
        EXPECT_EQ(AwaError_Success, AwaStaticClient_DefineResourceWithHandler(client_, "Test Boolean Resource",     writeDetail::TEST_OBJECT_NON_ARRAY_TYPES, writeDetail::TEST_RESOURCE_BOOLEAN,    AwaResourceType_Boolean,    0, 1, AwaResourceOperations_ReadWrite, handler));
        EXPECT_EQ(AwaError_Success, AwaStaticClient_DefineResourceWithHandler(client_, "Test Opaque Resource",      writeDetail::TEST_OBJECT_NON_ARRAY_TYPES, writeDetail::TEST_RESOURCE_OPAQUE,     AwaResourceType_Opaque,     0, 1, AwaResourceOperations_ReadWrite, handler));
        EXPECT_EQ(AwaError_Success, AwaStaticClient_DefineResourceWithHandler(client_, "Test Time Resource",        writeDetail::TEST_OBJECT_NON_ARRAY_TYPES, writeDetail::TEST_RESOURCE_TIME,       AwaResourceType_Time,       0, 1, AwaResourceOperations_ReadWrite, handler));
        EXPECT_EQ(AwaError_Success, AwaStaticClient_DefineResourceWithHandler(client_, "Test Object Link Resource", writeDetail::TEST_OBJECT_NON_ARRAY_TYPES, writeDetail::TEST_RESOURCE_OBJECTLINK, AwaResourceType_ObjectLink, 0, 1, AwaResourceOperations_ReadWrite, handler));
        EXPECT_EQ(AwaError_Success, AwaStaticClient_CreateObjectInstance(client_, writeDetail::TEST_OBJECT_NON_ARRAY_TYPES, 0));

        AwaServerListClientsOperation * operation = AwaServerListClientsOperation_New(session_);
        EXPECT_TRUE(NULL != operation);
        SingleStaticClientPollCondition condition(client_, operation, global::clientEndpointName, 10);
        EXPECT_TRUE(condition.Wait());
        AwaServerListClientsOperation_Free(&operation);

        AwaServerDefineOperation * serverDefineOperation = AwaServerDefineOperation_New(session_);
        EXPECT_TRUE(serverDefineOperation != NULL);

        AwaObjectDefinition * customObjectDefinition = AwaObjectDefinition_New(writeDetail::TEST_OBJECT_NON_ARRAY_TYPES, "Test Object Single", 0, 1);
        EXPECT_TRUE(NULL != customObjectDefinition);

        EXPECT_EQ(AwaError_Success, AwaObjectDefinition_AddResourceDefinitionAsString     (customObjectDefinition, writeDetail::TEST_RESOURCE_STRING,     "Test String Resource",      false, AwaResourceOperations_ReadWrite, NULL));
        EXPECT_EQ(AwaError_Success, AwaObjectDefinition_AddResourceDefinitionAsInteger    (customObjectDefinition, writeDetail::TEST_RESOURCE_INTEGER,    "Test Integer Resource",     false, AwaResourceOperations_ReadWrite, 0));
        EXPECT_EQ(AwaError_Success, AwaObjectDefinition_AddResourceDefinitionAsFloat      (customObjectDefinition, writeDetail::TEST_RESOURCE_FLOAT,      "Test Float Resource",       false, AwaResourceOperations_ReadWrite, 0.0));
        EXPECT_EQ(AwaError_Success, AwaObjectDefinition_AddResourceDefinitionAsBoolean    (customObjectDefinition, writeDetail::TEST_RESOURCE_BOOLEAN,    "Test Boolean Resource",     false, AwaResourceOperations_ReadWrite, false));
        EXPECT_EQ(AwaError_Success, AwaObjectDefinition_AddResourceDefinitionAsOpaque     (customObjectDefinition, writeDetail::TEST_RESOURCE_OPAQUE,     "Test Opaque Resource",      false, AwaResourceOperations_ReadWrite, AwaOpaque {0}));
        EXPECT_EQ(AwaError_Success, AwaObjectDefinition_AddResourceDefinitionAsTime       (customObjectDefinition, writeDetail::TEST_RESOURCE_TIME,       "Test Time Resource",        false, AwaResourceOperations_ReadWrite, 0));
        EXPECT_EQ(AwaError_Success, AwaObjectDefinition_AddResourceDefinitionAsObjectLink (customObjectDefinition, writeDetail::TEST_RESOURCE_OBJECTLINK, "Test Object Link Resource", false, AwaResourceOperations_ReadWrite, AwaObjectLink {0}));

        EXPECT_EQ(AwaError_Success, AwaServerDefineOperation_Add(serverDefineOperation, customObjectDefinition));
        AwaObjectDefinition_Free(&customObjectDefinition);

        customObjectDefinition = AwaObjectDefinition_New(writeDetail::TEST_OBJECT_ARRAY_TYPES, "Test Object Array", 0, 1);
        EXPECT_TRUE(NULL != customObjectDefinition);

        EXPECT_EQ(AwaError_Success, AwaObjectDefinition_AddResourceDefinitionAsStringArray    (customObjectDefinition, writeDetail::TEST_RESOURCE_STRING,     "Test String Array Resource",      0,5, AwaResourceOperations_ReadWrite, NULL));
        EXPECT_EQ(AwaError_Success, AwaObjectDefinition_AddResourceDefinitionAsIntegerArray   (customObjectDefinition, writeDetail::TEST_RESOURCE_INTEGER,    "Test Integer Array Resource",     0,5, AwaResourceOperations_ReadWrite, NULL));
        EXPECT_EQ(AwaError_Success, AwaObjectDefinition_AddResourceDefinitionAsFloatArray     (customObjectDefinition, writeDetail::TEST_RESOURCE_FLOAT,      "Test Float Array Resource",       0,5, AwaResourceOperations_ReadWrite, NULL));
        EXPECT_EQ(AwaError_Success, AwaObjectDefinition_AddResourceDefinitionAsBooleanArray   (customObjectDefinition, writeDetail::TEST_RESOURCE_BOOLEAN,    "Test Boolean Array Resource",     0,5, AwaResourceOperations_ReadWrite, NULL));
        EXPECT_EQ(AwaError_Success, AwaObjectDefinition_AddResourceDefinitionAsOpaqueArray    (customObjectDefinition, writeDetail::TEST_RESOURCE_OPAQUE,     "Test Opaque Array Resource",      0,5, AwaResourceOperations_ReadWrite, NULL));
        EXPECT_EQ(AwaError_Success, AwaObjectDefinition_AddResourceDefinitionAsTimeArray      (customObjectDefinition, writeDetail::TEST_RESOURCE_TIME,       "Test Time Array Resource",        0,5, AwaResourceOperations_ReadWrite, NULL));
        EXPECT_EQ(AwaError_Success, AwaObjectDefinition_AddResourceDefinitionAsObjectLinkArray(customObjectDefinition, writeDetail::TEST_RESOURCE_OBJECTLINK, "Test Object Link Array Resource", 0,5, AwaResourceOperations_ReadWrite, NULL));

        EXPECT_EQ(AwaError_Success, AwaServerDefineOperation_Add(serverDefineOperation, customObjectDefinition));
        EXPECT_EQ(AwaError_Success, AwaServerDefineOperation_Perform(serverDefineOperation, defaults::timeout));

        AwaObjectDefinition_Free(&customObjectDefinition);
        AwaServerDefineOperation_Free(&serverDefineOperation);

        writeOperation_ = AwaServerWriteOperation_New(session_, AwaWriteMode_Update);
        EXPECT_TRUE(NULL != writeOperation_);

        readOperation_ = AwaServerReadOperation_New(session_);
        EXPECT_TRUE(NULL != readOperation_);
    }

    void TearDown() {
        AwaServerWriteOperation_Free(&writeOperation_);
        AwaServerReadOperation_Free(&readOperation_);
        delete cbHandler;
        TestStaticClientWithServer::TearDown();
    }

    AwaClientSession * clientSession_;
    AwaServerSession * serverSession_;
    AwaClientDaemonHorde * horde_;
    AwaServerWriteOperation * writeOperation_;
    AwaServerReadOperation * readOperation_;
};

AwaResult TestWriteValueStaticClient_WriteHandler(void * context, AwaOperation operation, AwaObjectID objectID, AwaObjectInstanceID objectInstanceID, AwaResourceID resourceID, AwaResourceInstanceID resourceInstanceID, void ** dataPointer, size_t * dataSize, bool * changed)
{
    TestWriteReadStaticResource * data = static_cast<TestWriteReadStaticResource *>(context);
    EXPECT_EQ(AwaOperation_Write, operation);
    EXPECT_EQ(data->ObjectID, objectID);
    EXPECT_EQ(data->ObjectInstanceID, objectInstanceID);
    EXPECT_EQ(data->ResourceID, resourceID);
    EXPECT_EQ(0, resourceInstanceID);

    EXPECT_TRUE(dataSize != NULL);
    EXPECT_TRUE(dataPointer != NULL);

    EXPECT_EQ(static_cast<size_t>(data->ValueSize), *dataSize);
    EXPECT_EQ(0, memcmp(data->Value, *dataPointer, data->ValueSize));

    data->Complete = true;
    return AwaResult_SuccessChanged;
}


AwaResult TestWriteValueStaticClient_ReadHandler(void * context, AwaOperation operation, AwaObjectID objectID, AwaObjectInstanceID objectInstanceID, AwaResourceID resourceID, AwaResourceInstanceID resourceInstanceID, void ** dataPointer, size_t * dataSize, bool * changed)
{
    TestWriteReadStaticResource * data = static_cast<TestWriteReadStaticResource *>(context);

    EXPECT_EQ(AwaOperation_Read, operation);
    EXPECT_EQ(data->ObjectID, objectID);
    EXPECT_EQ(data->ObjectInstanceID, objectInstanceID);
    EXPECT_EQ(data->ResourceID, resourceID);
    EXPECT_EQ(0, resourceInstanceID);

    EXPECT_TRUE(dataSize != NULL);
    EXPECT_TRUE(dataPointer != NULL);

    *dataPointer = (void*)data->Value;
    *dataSize = data->ValueSize;

    if(data->TestRead == true)
    {
        data->Complete = true;
    }

    return AwaResult_SuccessContent;
}

TEST_P(TestStaticClientWriteReadValue, TestWriteReadValueSingle)
{
    TestWriteReadStaticResource data = GetParam();
    char path[128] = {0};

    EXPECT_EQ(AwaError_Success, AwaAPI_MakePath(path, sizeof(path), data.ObjectID, data.ObjectInstanceID, data.ResourceID));

    switch (data.Type)
    {
        case AwaResourceType_String:
            EXPECT_EQ(AwaError_Success, AwaServerWriteOperation_AddValueAsCString(writeOperation_, path, (const char *)data.Value));
            break;
        case AwaResourceType_Integer:
            EXPECT_EQ(AwaError_Success, AwaServerWriteOperation_AddValueAsInteger(writeOperation_, path, *((AwaInteger*)data.Value)));
            break;
        case AwaResourceType_Float:
            EXPECT_EQ(AwaError_Success, AwaServerWriteOperation_AddValueAsFloat(writeOperation_, path, *((AwaFloat*)data.Value)));
            break;
        case AwaResourceType_Boolean:
            EXPECT_EQ(AwaError_Success, AwaServerWriteOperation_AddValueAsBoolean(writeOperation_, path, *((AwaBoolean*)data.Value)));
            break;
        case AwaResourceType_Opaque:
            {
                AwaOpaque opaque = { (void *)data.Value, static_cast<size_t>(data.ValueSize) };
                EXPECT_EQ(AwaError_Success, AwaServerWriteOperation_AddValueAsOpaque(writeOperation_, path, opaque));
                break;
            }
        case AwaResourceType_Time:
            EXPECT_EQ(AwaError_Success, AwaServerWriteOperation_AddValueAsTime(writeOperation_, path, *((AwaTime*)data.Value)));
            break;
        case AwaResourceType_ObjectLink:
            {
                AwaObjectLink * objectlink = (AwaObjectLink *)data.Value;
                EXPECT_EQ(AwaError_Success, AwaServerWriteOperation_AddValueAsObjectLink(writeOperation_, path, *objectlink));
                break;
            }
        default:
            EXPECT_TRUE(false);
            break;
    }

    pthread_t writeThread;
    pthread_create(&writeThread, NULL, do_write_operation, (void *)writeOperation_);
    ASSERT_TRUE(cbHandler->Wait());
    pthread_join(writeThread, NULL);

    cbHandler->data.Complete = false;
    cbHandler->data.TestRead = true;
    cbHandler->Reset();

    AwaServerReadOperation_AddPath(readOperation_, global::clientEndpointName, path);

    pthread_t readThread;
    pthread_create(&readThread, NULL, do_read_operation, (void *)readOperation_);
    ASSERT_TRUE(cbHandler->Wait());
    pthread_join(readThread, NULL);

    const AwaServerReadResponse * readResponse = AwaServerReadOperation_GetResponse(readOperation_, global::clientEndpointName );
    EXPECT_TRUE(readResponse != NULL);

    switch (data.Type)
    {
        case AwaResourceType_String:
            {
                const char * value;
                EXPECT_EQ(AwaError_Success, AwaServerReadResponse_GetValueAsCStringPointer(readResponse, path, (const char **)&value));
                ASSERT_STREQ((char*) data.Value, (char*) value);
                break;
            }
        case AwaResourceType_Integer:
            {
                AwaInteger * value;
                EXPECT_EQ(AwaError_Success, AwaServerReadResponse_GetValueAsIntegerPointer(readResponse, path, (const AwaInteger **)&value));
                ASSERT_EQ(*static_cast<const AwaInteger *>(data.Value), *static_cast<AwaInteger *>(value));
                break;
            }
        case AwaResourceType_Float:
            {
                AwaFloat * value;
                EXPECT_EQ(AwaError_Success, AwaServerReadResponse_GetValueAsFloatPointer(readResponse, path, (const AwaFloat **)&value));
                ASSERT_EQ(*static_cast<const AwaFloat *>(data.Value), *static_cast<AwaFloat *>(value));
                break;
            }
        case AwaResourceType_Boolean:
            {
                AwaBoolean * value;
                EXPECT_EQ(AwaError_Success, AwaServerReadResponse_GetValueAsBooleanPointer(readResponse, path, (const AwaBoolean **)&value));
                ASSERT_EQ(*static_cast<const AwaBoolean *>(data.Value), *static_cast<AwaBoolean *>(value));
                break;
            }
        case AwaResourceType_Opaque:
            {
                AwaOpaque * value;
                EXPECT_EQ(AwaError_Success, AwaServerReadResponse_GetValueAsOpaquePointer(readResponse, path, (const AwaOpaque **)&value));
                ASSERT_EQ(data.ValueSize, static_cast<int>(value->Size));
                ASSERT_TRUE(memcmp(value->Data, data.Value, data.ValueSize) == 0);
                break;
            }
        case AwaResourceType_Time:
            {
                AwaTime * value;
                EXPECT_EQ(AwaError_Success, AwaServerReadResponse_GetValueAsTimePointer(readResponse, path, (const AwaTime **)&value));
                ASSERT_EQ(*static_cast<const AwaTime *>(data.Value), *static_cast<AwaTime *>(value));
                break;
            }
        case AwaResourceType_ObjectLink:
            {
                AwaObjectLink * expectedObjectLink = (AwaObjectLink *)data.Value;
                const AwaObjectLink * receivedObjectLinkPointer;
                EXPECT_EQ(AwaError_Success, AwaServerReadResponse_GetValueAsObjectLinkPointer(readResponse, path, (const AwaObjectLink **)&receivedObjectLinkPointer));
                EXPECT_EQ(expectedObjectLink->ObjectID, receivedObjectLinkPointer->ObjectID);
                EXPECT_EQ(expectedObjectLink->ObjectInstanceID, receivedObjectLinkPointer->ObjectInstanceID);
                break;
            }
    default:
        EXPECT_TRUE(false);
        break;
    }

}

INSTANTIATE_TEST_CASE_P(
        TestStaticClientWriteReadValue,
        TestStaticClientWriteReadValue,
        ::testing::Values(
        TestWriteReadStaticResource {TestWriteValueStaticClient_WriteHandler, TestWriteValueStaticClient_ReadHandler, writeDetail::TEST_OBJECT_NON_ARRAY_TYPES, 0, writeDetail::TEST_RESOURCE_STRING,     writeDetail::dummyString1,      1, static_cast<int>(strlen(writeDetail::dummyString1)),     AwaResourceType_String,     true, false},
        TestWriteReadStaticResource {TestWriteValueStaticClient_WriteHandler, TestWriteValueStaticClient_ReadHandler, writeDetail::TEST_OBJECT_NON_ARRAY_TYPES, 0, writeDetail::TEST_RESOURCE_INTEGER,    &writeDetail::dummyInteger1,    1, static_cast<int>(sizeof(writeDetail::dummyInteger1)),    AwaResourceType_Integer,    true, false},
        TestWriteReadStaticResource {TestWriteValueStaticClient_WriteHandler, TestWriteValueStaticClient_ReadHandler, writeDetail::TEST_OBJECT_NON_ARRAY_TYPES, 0, writeDetail::TEST_RESOURCE_FLOAT,      &writeDetail::dummyFloat1,      1, static_cast<int>(sizeof(writeDetail::dummyFloat1)),      AwaResourceType_Float,      true, false},
        TestWriteReadStaticResource {TestWriteValueStaticClient_WriteHandler, TestWriteValueStaticClient_ReadHandler, writeDetail::TEST_OBJECT_NON_ARRAY_TYPES, 0, writeDetail::TEST_RESOURCE_BOOLEAN,    &writeDetail::dummyBoolean1,    1, static_cast<int>(sizeof(writeDetail::dummyBoolean1)),    AwaResourceType_Boolean,    true, false},
        TestWriteReadStaticResource {TestWriteValueStaticClient_WriteHandler, TestWriteValueStaticClient_ReadHandler, writeDetail::TEST_OBJECT_NON_ARRAY_TYPES, 0, writeDetail::TEST_RESOURCE_OPAQUE,     writeDetail::dummyOpaqueData,   1, static_cast<int>(sizeof(writeDetail::dummyOpaqueData)),  AwaResourceType_Opaque,     true, false},
        TestWriteReadStaticResource {TestWriteValueStaticClient_WriteHandler, TestWriteValueStaticClient_ReadHandler, writeDetail::TEST_OBJECT_NON_ARRAY_TYPES, 0, writeDetail::TEST_RESOURCE_TIME,       &writeDetail::dummyTime1,       1, static_cast<int>(sizeof(writeDetail::dummyTime1)),       AwaResourceType_Time,       true, false},
        TestWriteReadStaticResource {TestWriteValueStaticClient_WriteHandler, TestWriteValueStaticClient_ReadHandler, writeDetail::TEST_OBJECT_NON_ARRAY_TYPES, 0, writeDetail::TEST_RESOURCE_OBJECTLINK, &writeDetail::dummyObjectLink1, 1, static_cast<int>(sizeof(writeDetail::dummyObjectLink1)), AwaResourceType_ObjectLink, true, false}
        ));
} // namespace Awa