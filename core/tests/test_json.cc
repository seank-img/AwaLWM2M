#include <gtest/gtest.h>
#include <string>
#include <stdio.h>
#include <stdint.h>

// https://meekrosoft.wordpress.com/2009/11/09/unit-testing-c-code-with-the-googletest-framework/
// 1. Define fake functions for the dependencies you want to stub out
// 2. If the module depends on a global (gasp!) you need to define your fake one
// 3. include your module implementation (#include module.c)
// 4. Define a method to reset all the static data to a known state.
// 5. Define your tests

#include "common/lwm2m_object_store.h"
#include "common/lwm2m_json.c"
#include "common/lwm2m_tree_node.h"
#include "common/lwm2m_tree_builder.h"
#include "client/lwm2m_device_object.h"
#include "lwm2m_core.h"

class JsonTestSuite : public testing::Test
{
    void SetUp() { context = Lwm2mCore_Init(NULL, NULL); }
    void TearDown() { Lwm2mCore_Destroy(context); }

protected:
    Lwm2mContextType * context;
};


TEST_F(JsonTestSuite, test_deserialise_single_object_instance)
{
    const char * input = (char*)"{\"e\":[\n"
    "{\"n\":\"11/0\",\"v\":0},\n"
    "{\"n\":\"16\",\"sv\":\"U\"},\n"
    "{\"n\":\"0\",\"sv\":\"Imagination Technologies\"},\n"
    "{\"n\":\"1\",\"sv\":\"Awa Client\"},\n"
    "{\"n\":\"2\",\"sv\":\"SN12345678\"},\n"
    "{\"n\":\"3\",\"sv\":\"0.1a\"},\n"
    "{\"n\":\"6/0\",\"v\":1},\n"
    "{\"n\":\"6/1\",\"v\":5},\n"
    "{\"n\":\"7/0\",\"v\":3800},\n"
    "{\"n\":\"7/1\",\"v\":5000},\n"
    "{\"n\":\"8/0\",\"v\":125},\n"
    "{\"n\":\"8/1\",\"v\":900},\n"
    "{\"n\":\"9\",\"v\":100},\n"
    "{\"n\":\"10\",\"v\":15},\n"
    "{\"n\":\"13\",\"v\":2718619435},\n"
    "{\"n\":\"14\",\"sv\":\"+12:00\"},\n"
    "{\"n\":\"15\",\"sv\":\"Pacific/Wellington\"},\n"
    "{\"n\":\"17\",\"sv\":\"AwaClient\"},\n"
    "{\"n\":\"18\",\"sv\":\"0.0.0.1\"},\n"
    "{\"n\":\"19\",\"sv\":\"0.0.0.11\"},\n"
    "{\"n\":\"20\",\"v\":2},\n"
    "{\"n\":\"21\",\"v\":42}]\n"
    "}";

    int inputSize = strlen(input) + 1;
    int objectID = 3;
    int objectInstanceID = 0;

    Lwm2m_RegisterDeviceObject(context);

    Lwm2mTreeNode * dest;
    SerdesContext serdesContext;
    int len = JsonDeserialiseObjectInstance(&serdesContext, &dest, Lwm2mCore_GetDefinitions(context), objectID, objectInstanceID, (const uint8_t *)input, inputSize);
    EXPECT_EQ(0, len);
    EXPECT_EQ(19, Lwm2mTreeNode_GetChildCount(dest));
    Lwm2mTreeNode * child = Lwm2mTreeNode_GetFirstChild(dest);
    while (child)
    {
        int childID;
        Lwm2mTreeNode_GetID(child, &childID);
        Lwm2mTreeNode * resourceInstance = Lwm2mTreeNode_GetFirstChild(child);
        uint16_t valueLength;
        const uint8_t * value = Lwm2mTreeNode_GetValue(resourceInstance, &valueLength);
        switch (childID)
        {
        case 0:
            printf("value: %s\n", value);
            EXPECT_EQ(0, memcmp("Imagination Technologies", (const char *) value, strlen("Imagination Technologies")));
            break;
        case 1:
            EXPECT_EQ(0, memcmp("Awa Client", (const char *) value, strlen("Awa Client")));
            break;
        case 13:
            EXPECT_EQ(0xA20AD72B, *((const uint64_t*)value));
            break;
        default:
            // assume the rest are OK
            EXPECT_GE(childID, 0);
            EXPECT_LE(childID, 21);
            break;
        }

        child = Lwm2mTreeNode_GetNextChild(dest, child);
    }

    Lwm2mTreeNode_DeleteRecursive(dest);
}

TEST_F(JsonTestSuite, test_serialise_string)
{
    Lwm2m_SetLogLevel(DebugLevel_Debug);

    Definition_RegisterObjectType(Lwm2mCore_GetDefinitions(context), (char*)"Test", 0, MultipleInstancesEnum_Single, MandatoryEnum_Mandatory, &defaultObjectOperationHandlers);
    Lwm2mCore_RegisterResourceType(context, (char*)"Res1", 0, 0, AwaResourceType_String, MultipleInstancesEnum_Single, MandatoryEnum_Mandatory, AwaResourceOperations_ReadWrite, &defaultResourceOperationHandlers);
    Lwm2mCore_CreateObjectInstance(context, 0, 0);
    Lwm2mCore_SetResourceInstanceValue(context, 0, 0, 0, 0, (char*)"Open Mobile Alliance", strlen("Open Mobile Alliance"));

    uint8_t buffer[512];
    memset(buffer, 0, 512);

    char * expected = (char*)"{\"e\":[\n"
    "{\"n\":\"0/0\",\"sv\":\"Open Mobile Alliance\"}]\n"
    "}\n";

    Lwm2mTreeNode * dest;
    TreeBuilder_CreateTreeFromObject(&dest, context, Lwm2mRequestOrigin_Client, 0);

    SerdesContext serdesContext = NULL;
    int len = JsonSerialiseObject(&serdesContext, dest, 0, buffer, sizeof(buffer));

    Lwm2mTreeNode_DeleteRecursive(dest);

    EXPECT_EQ(static_cast<int>(strlen(expected)), len);
    EXPECT_EQ(0, memcmp(buffer, expected, strlen(expected)));
}

TEST_F(JsonTestSuite, test_serialise_string_multiple)
{
    Lwm2m_SetLogLevel(DebugLevel_Debug);

    Definition_RegisterObjectType(Lwm2mCore_GetDefinitions(context), (char*)"Test", 0, MultipleInstancesEnum_Single, MandatoryEnum_Mandatory, &defaultObjectOperationHandlers);
    Lwm2mCore_RegisterResourceType(context, (char*)"Res1", 0, 0, AwaResourceType_String, MultipleInstancesEnum_Single, MandatoryEnum_Mandatory, AwaResourceOperations_ReadWrite, &defaultResourceOperationHandlers);
    Lwm2mCore_RegisterResourceType(context, (char*)"Res2", 0, 1, AwaResourceType_String, MultipleInstancesEnum_Single, MandatoryEnum_Mandatory, AwaResourceOperations_ReadWrite, &defaultResourceOperationHandlers);
    Lwm2mCore_CreateObjectInstance(context, 0, 0);
    Lwm2mCore_SetResourceInstanceValue(context, 0, 0, 0, 0, (char*)"Open Mobile Alliance", strlen("Open Mobile Alliance"));
    Lwm2mCore_SetResourceInstanceValue(context, 0, 0, 1, 0, (char*)"Lightweight M2M Client", strlen("Lightweight M2M Client"));

    uint8_t buffer[512];
    memset(buffer, 0, 512);

    char * expected = (char*)"{\"e\":[\n"
    "{\"n\":\"0/0\",\"sv\":\"Open Mobile Alliance\"},\n"
    "{\"n\":\"0/1\",\"sv\":\"Lightweight M2M Client\"}]\n"
    "}\n";

    Lwm2mTreeNode * dest;

    TreeBuilder_CreateTreeFromObject(&dest, context, Lwm2mRequestOrigin_Client, 0);

    SerdesContext serdesContext = NULL;
    int len = JsonSerialiseObject(&serdesContext, dest, 0, buffer, sizeof(buffer));

    Lwm2mTreeNode_DeleteRecursive(dest);

    EXPECT_EQ(static_cast<int>(strlen(expected)), len);
    EXPECT_EQ(0, memcmp(buffer, expected, strlen(expected))) << std::endl << expected << " != "<< std::endl << buffer;
}

TEST_F(JsonTestSuite, test_serialise_opaque)
{
    Lwm2m_SetLogLevel(DebugLevel_Debug);

    char * decoded = (char*)"Open Mobile Alliance";
    //char * encoded = "T3BlbiBNb2JpbGUgQWxsaWFuY2U=";

    Definition_RegisterObjectType(Lwm2mCore_GetDefinitions(context), (char*)"Test", 0, MultipleInstancesEnum_Single, MandatoryEnum_Mandatory, &defaultObjectOperationHandlers);
    Lwm2mCore_RegisterResourceType(context, (char*)"Res1", 0, 0, AwaResourceType_Opaque, MultipleInstancesEnum_Single, MandatoryEnum_Mandatory, AwaResourceOperations_ReadWrite, &defaultResourceOperationHandlers);
    Lwm2mCore_CreateObjectInstance(context, 0, 0);
    Lwm2mCore_SetResourceInstanceValue(context, 0, 0, 0, 0, decoded, strlen(decoded));

    uint8_t buffer[512];
    memset(buffer, 0, 512);

    char * expected = (char*)"{\"e\":[\n"
    "{\"n\":\"0/0\",\"sv\":\"T3BlbiBNb2JpbGUgQWxsaWFuY2U=\"}]\n"
    "}\n";

    Lwm2mTreeNode * dest;

    TreeBuilder_CreateTreeFromObject(&dest, context, Lwm2mRequestOrigin_Client, 0);
  
    SerdesContext serdesContext = NULL;
    int len = JsonSerialiseObject(&serdesContext, dest, 0, buffer, sizeof(buffer));

    Lwm2mTreeNode_DeleteRecursive(dest);

    EXPECT_EQ(static_cast<int>(strlen(expected)), len);
    EXPECT_EQ(0, memcmp(buffer, expected, strlen(expected)));
}

TEST_F(JsonTestSuite, test_serialise_boolean)
{
    Lwm2m_SetLogLevel(DebugLevel_Debug);

    bool value = true;
    //char * encoded = "T3BlbiBNb2JpbGUgQWxsaWFuY2U=";

    Definition_RegisterObjectType(Lwm2mCore_GetDefinitions(context), (char*)"Test", 0, MultipleInstancesEnum_Single, MandatoryEnum_Mandatory, &defaultObjectOperationHandlers);
    Lwm2mCore_RegisterResourceType(context, (char*)"Res1", 0, 0, AwaResourceType_Boolean, MultipleInstancesEnum_Single, MandatoryEnum_Mandatory, AwaResourceOperations_ReadWrite, &defaultResourceOperationHandlers);
    Lwm2mCore_CreateObjectInstance(context, 0, 0);
    Lwm2mCore_SetResourceInstanceValue(context, 0, 0, 0, 0, &value, sizeof(value));

    uint8_t buffer[512];
    memset(buffer, 0, 512);

    char * expected = (char*)"{\"e\":[\n"
    "{\"n\":\"0/0\",\"bv\":\"true\"}]\n"
    "}\n";

    Lwm2mTreeNode * dest;

    TreeBuilder_CreateTreeFromObject(&dest, context, Lwm2mRequestOrigin_Client, 0);

    //TODO: change other tests to use single instance of an object.
    SerdesContext serdesContext = NULL;
    int len = JsonSerialiseObject(&serdesContext, dest, 0, buffer, sizeof(buffer));

    Lwm2mTreeNode_DeleteRecursive(dest);

    EXPECT_EQ(static_cast<int>(strlen(expected)), len) << expected << std::endl << buffer;
    EXPECT_EQ(0, memcmp(buffer, expected, strlen(expected)));
}

TEST_F(JsonTestSuite, test_serialise_float)
{
    Lwm2m_SetLogLevel(DebugLevel_Debug);

    double value = 5.23;
    //char * encoded = "T3BlbiBNb2JpbGUgQWxsaWFuY2U=";

    Definition_RegisterObjectType(Lwm2mCore_GetDefinitions(context), (char*)"Test", 0, MultipleInstancesEnum_Single, MandatoryEnum_Mandatory, &defaultObjectOperationHandlers);
    Lwm2mCore_RegisterResourceType(context, (char*)"Res1", 0, 0, AwaResourceType_Float, MultipleInstancesEnum_Single, MandatoryEnum_Mandatory, AwaResourceOperations_ReadWrite, &defaultResourceOperationHandlers);
    Lwm2mCore_CreateObjectInstance(context, 0, 0);
    Lwm2mCore_SetResourceInstanceValue(context, 0, 0, 0, 0, &value, sizeof(value));

    uint8_t buffer[512];
    memset(buffer, 0, 512);

    char * expected = (char*)"{\"e\":[\n"
    "{\"n\":\"0/0\",\"v\":5.230000}]\n"
    "}\n";

    Lwm2mTreeNode * dest;

    TreeBuilder_CreateTreeFromObject(&dest, context, Lwm2mRequestOrigin_Client, 0);

    SerdesContext serdesContext = NULL;
    int len = JsonSerialiseObject(&serdesContext, dest, 0, buffer, sizeof(buffer));

    Lwm2mTreeNode_DeleteRecursive(dest);

    EXPECT_EQ(static_cast<int>(strlen(expected)), len) << expected << std::endl << buffer;
    EXPECT_EQ(0, memcmp(buffer, expected, strlen(expected)));
}

TEST_F(JsonTestSuite, test_serialise_integer)
{
    Lwm2m_SetLogLevel(DebugLevel_Debug);

    int64_t value = 9001;

    Definition_RegisterObjectType(Lwm2mCore_GetDefinitions(context), (char*)"Test", 0, MultipleInstancesEnum_Single, MandatoryEnum_Mandatory, &defaultObjectOperationHandlers);
    Lwm2mCore_RegisterResourceType(context, (char*)"Res1", 0, 0, AwaResourceType_Integer, MultipleInstancesEnum_Single, MandatoryEnum_Mandatory, AwaResourceOperations_ReadWrite, &defaultResourceOperationHandlers);
    Lwm2mCore_CreateObjectInstance(context, 0, 0);
    Lwm2mCore_SetResourceInstanceValue(context, 0, 0, 0, 0, &value, sizeof(value));

    uint8_t buffer[512];
    memset(buffer, 0, 512);

    char * expected = (char*)"{\"e\":[\n"
    "{\"n\":\"0/0\",\"v\":9001}]\n"
    "}\n";

    Lwm2mTreeNode * dest;

    TreeBuilder_CreateTreeFromObject(&dest, context, Lwm2mRequestOrigin_Client, 0);

    SerdesContext serdesContext = NULL;
    int len = JsonSerialiseObject(&serdesContext, dest, 0, buffer, sizeof(buffer));
 
    Lwm2mTreeNode_DeleteRecursive(dest);

    EXPECT_EQ(static_cast<int>(strlen(expected)), len) << expected << std::endl << buffer;
    EXPECT_EQ(0, memcmp(buffer, expected, strlen(expected)));
}

TEST_F(JsonTestSuite, test_serialise_device_object)
{
    Lwm2m_SetLogLevel(DebugLevel_Debug);

    Lwm2m_RegisterDeviceObject(context);


    uint8_t buffer[1024];
    memset(buffer, 0, 1024);

    char * expected = (char*)"{\"e\":[\n"
    "{\"n\":\"11/0\",\"v\":0},\n"
    "{\"n\":\"16\",\"sv\":\"U\"},\n"
    "{\"n\":\"0\",\"sv\":\"Imagination Technologies\"},\n"
    "{\"n\":\"1\",\"sv\":\"Awa Client\"},\n"
    "{\"n\":\"2\",\"sv\":\"SN12345678\"},\n"
    "{\"n\":\"3\",\"sv\":\"0.1a\"},\n"
    "{\"n\":\"6/0\",\"v\":1},\n"
    "{\"n\":\"6/1\",\"v\":5},\n"
    "{\"n\":\"7/0\",\"v\":3800},\n"
    "{\"n\":\"7/1\",\"v\":5000},\n"
    "{\"n\":\"8/0\",\"v\":125},\n"
    "{\"n\":\"8/1\",\"v\":900},\n"
    "{\"n\":\"9\",\"v\":100},\n"
    "{\"n\":\"10\",\"v\":15},\n"
    "{\"n\":\"13\",\"v\":2718619435},\n"
    "{\"n\":\"14\",\"sv\":\"+12:00\"},\n"
    "{\"n\":\"15\",\"sv\":\"Pacific/Wellington\"},\n"
    "{\"n\":\"17\",\"sv\":\"AwaClient\"},\n"
    "{\"n\":\"18\",\"sv\":\"0.0.0.1\"},\n"
    "{\"n\":\"19\",\"sv\":\"0.0.0.11\"},\n"
    "{\"n\":\"20\",\"v\":2},\n"
    "{\"n\":\"21\",\"v\":42}]\n"
    "}\n";

    Lwm2mTreeNode * dest;

    TreeBuilder_CreateTreeFromObjectInstance(&dest, context, Lwm2mRequestOrigin_Client, 3, 0);

    SerdesContext serdesContext = NULL;
    int len = JsonSerialiseObjectInstance(&serdesContext, dest, 3, 0, buffer, sizeof(buffer));

    Lwm2mTreeNode_DeleteRecursive(dest);

    printf("%s", &expected[35]);

    EXPECT_EQ(static_cast<int>(strlen(expected)), len) << expected << std::endl << buffer;
    EXPECT_EQ(0, memcmp(buffer, expected, strlen(expected)));
}





