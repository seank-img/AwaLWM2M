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

class TestStaticClientCreateDeleteWithServer : public TestStaticClientWithServer {};

TEST_F(TestStaticClientCreateDeleteWithServer, AwaStaticClient_CreateDelete_object_valid_inputs)
{
	AwaInteger i = 0;
    EXPECT_EQ(AwaError_Success, AwaStaticClient_DefineObject(client_, "TestObject", 7997, 0, 1)); // valid
    EXPECT_EQ(AwaError_Success, AwaStaticClient_DefineResourceWithPointer(client_, "Resource", 7997,  1, AwaResourceType_Integer, 0, 1, AwaResourceOperations_ReadWrite, &i, sizeof(i), 0));


    AwaServerListClientsOperation * operation = AwaServerListClientsOperation_New(session_);
    EXPECT_TRUE(NULL != operation);

    SingleStaticClientPollCondition bootstrap_condition(client_, operation, global::clientEndpointName, 20);
    ASSERT_TRUE(bootstrap_condition.Wait());

    EXPECT_EQ(AwaError_Success, AwaStaticClient_CreateObjectInstance(client_, 7997, 0));

    SingleStaticClientObjectPollCondition create_condition(client_, operation, global::clientEndpointName, "/7997/0", 20);
    ASSERT_TRUE(create_condition.Wait());

    EXPECT_EQ(AwaError_Success, AwaStaticClient_DeleteObjectInstance(client_, 7997, 0));

    SingleStaticClientObjectPollCondition delete_condition(client_, operation, global::clientEndpointName, "/7997/0", 20, true);
    ASSERT_TRUE(delete_condition.Wait());

    AwaServerListClientsOperation_Free(&operation);
}

} // namespace Awa