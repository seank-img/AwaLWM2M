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


#include <string>
#include <map>
#include <gtest/gtest.h>
#include <stdio.h>
#include <stdint.h>
#include <arpa/inet.h>
#include <poll.h>

#pragma GCC diagnostic ignored "-Wunused-variable"
#pragma GCC diagnostic ignored "-Wsign-compare"
#define WITH_POSIX
#include <coap.h>
#include <address.h>
#pragma GCC diagnostic pop
#pragma GCC diagnostic pop


#include "common/coap_abstraction.h"
#include "client/lwm2m_core.h"


static void libcoap_HandleRequest(coap_context_t * ctx,
        struct coap_resource_t * resource,
        const coap_endpoint_t * local_interface,
        coap_address_t * peer,
        coap_pdu_t * request,
        str * token,
        coap_pdu_t * response);

class LibCoapEndpointCallbacks
{
public:
    const unsigned char * path_;
    coap_resource_t * r_;
    bool done;

    virtual void HandleRequest(coap_context_t * ctx,
            struct coap_resource_t * resource,
            const coap_endpoint_t * local_interface,
            coap_address_t * peer,
            coap_pdu_t * request,
            str * token,
            coap_pdu_t * response) {};
};

class EndpointCallbackMap
{
public:
    typedef std::pair<const unsigned char *, LibCoapEndpointCallbacks *> MapItem;
    typedef std::map<const unsigned char *, LibCoapEndpointCallbacks *> MapType;

    EndpointCallbackMap() : map_() {}


    void AddEndpoint(LibCoapEndpointCallbacks * endpoint)
    {
        map_.insert(MapItem(endpoint->path_, endpoint));
    }

    LibCoapEndpointCallbacks * GetEndpoint(unsigned char * path)
    {

        LibCoapEndpointCallbacks * endpoint = NULL;

        try
        {
             endpoint = map_.at(path);
        }
        catch(const std::out_of_range& oor)
        {
            endpoint = NULL;
        }

        return endpoint;
    }

protected:
    MapType map_;
};

EndpointCallbackMap endpointMap;


class LibCoapEndpoint : public LibCoapEndpointCallbacks
{
public:


   LibCoapEndpoint(coap_context_t * context, const unsigned char * path)
   {
       path_ = path;
       r_ = coap_resource_init(path, strlen((const char *)path), 0);
       coap_register_handler(r_, COAP_REQUEST_GET, libcoap_HandleRequest);
       coap_register_handler(r_, COAP_REQUEST_POST, libcoap_HandleRequest);
       coap_register_handler(r_, COAP_REQUEST_PUT, libcoap_HandleRequest);
       coap_register_handler(r_, COAP_REQUEST_DELETE, libcoap_HandleRequest);
       coap_add_resource(context, r_);
       endpointMap.AddEndpoint(this);
   }


   int GetRequestType(coap_pdu_t * request)
   {
       return request->hdr->code;
   }

   std::string GetRequestURIPath(coap_pdu_t * request)
   {
       std::string requestPath = "";
       coap_opt_iterator_t opt_iter;
       coap_opt_filter_t filter;
       coap_opt_t *option;

       coap_option_filter_clear(filter);
       coap_option_setb(filter, COAP_OPTION_URI_PATH);
       coap_option_iterator_init(request, &opt_iter, filter);

       while ((option = coap_option_next(&opt_iter)) != NULL)
       {
           char op[128] = {0};
           memcpy(op, COAP_OPT_VALUE(option), COAP_OPT_LENGTH(option));
           requestPath = requestPath + op;
       }

       return requestPath;
   }

   std::string GetRequestURIQuery(coap_pdu_t * request)
   {
       std::string requestQuery = "";
       coap_opt_iterator_t opt_iter;
       coap_opt_filter_t filter;
       coap_opt_t *option;
       bool first = true;

       coap_option_filter_clear(filter);
       coap_option_setb(filter, COAP_OPTION_URI_QUERY);
       coap_option_iterator_init(request, &opt_iter, filter);

       while ((option = coap_option_next(&opt_iter)))
       {
           char query[128] = {0};
           memcpy(query, COAP_OPT_VALUE(option), COAP_OPT_LENGTH(option));
           if (!first)
           {
               requestQuery = requestQuery + "&" + query;
           }
           else
           {
               requestQuery = requestQuery + "?" + query;
               first = false;
           }
       }

       return requestQuery;
   }


};


static void libcoap_HandleRequest(coap_context_t * ctx,
        struct coap_resource_t * resource,
        const coap_endpoint_t * local_interface,
        coap_address_t * peer,
        coap_pdu_t * request,
        str * token,
        coap_pdu_t * response)
{
    LibCoapEndpointCallbacks * endpoint = endpointMap.GetEndpoint(resource->uri.s);

    if (endpoint != NULL)
    {
        endpoint->HandleRequest(ctx, resource, local_interface, peer, request, token, response);
    }
}

class LibCoapServer
{

public:
    LibCoapServer(std::string hostname, int port)
    {
        coap_address_t addr;

        coap_address_init(&addr);

        addr.addr.sin.sin_port = htons(port);
        addr.size = sizeof(struct sockaddr_in);
        addr.addr.sin.sin_addr.s_addr = inet_addr(hostname.c_str());
        addr.addr.sa.sa_family = AF_INET;

        libcoapContext = coap_new_context(&addr);

        EXPECT_TRUE(libcoapContext != NULL);
    }

    ~LibCoapServer()
    {
        coap_free_context(libcoapContext);
    }

//    void CoapProcess()
//    {
//        coap_queue_t *nextpdu;
//        coap_tick_t now;
//
//        nextpdu = coap_peek_next( libcoapContext );
//
//        coap_ticks(&now);
//        while (nextpdu && (nextpdu->t <= now - libcoapContext->sendqueue_basetime))
//        {
//            coap_queue_t * node = coap_pop_next( libcoapContext );
//
//            coap_tid_t tid = coap_retransmit(libcoapContext, node);
//
//            nextpdu = coap_peek_next( libcoapContext );
//        }
//    }

    bool CoapHandleMessages(int timeout)
    {
        bool pollTimeout = false;
        running = true;

        do
        {
            int loop_result;
            struct pollfd fds[1];
            int nfds = 1;

            fds[0].fd = libcoapContext->sockfd;
            fds[0].events = POLLIN;

            loop_result = poll(fds, nfds, timeout*1000);

            if (loop_result < 0)
            {
                if (errno == EINTR)
                {
                    pollTimeout = true;
                    continue;
                }

                perror("poll:");
                break;
            }
            else if (loop_result > 0)
            {
                if (fds[0].revents == POLLIN)
                {
                    coap_read(libcoapContext);
                }
            }
            else
            {
                pollTimeout = true;
            }
        }
        while(!pollTimeout && running);

        return !pollTimeout;
    }

    coap_context_t * libcoapContext;
    bool running;
};

class CoapClientTestWithLibCoapServer : public testing::Test
{
public:
    LibCoapServer * libCoapServer;
};


TEST_F(CoapClientTestWithLibCoapServer, test_coap_Init)
{
    CoapInfo * coapInfo = coap_Init("127.0.0.1", 5687, 0);
    EXPECT_TRUE(coapInfo != NULL);

    coap_Destroy();
}


void ClientTransactionCallback(void * context, AddressType * addr, const char * responsePath, int responseCode, ContentType contentType, char * payload, size_t payloadLen)
{
    printf("callback...\n");
}

TEST_F(CoapClientTestWithLibCoapServer, test_coap_Post)
{
    CoapInfo * coapInfo = coap_Init("127.0.0.1", 5687, 3);
    EXPECT_TRUE(coapInfo != NULL);

    int serverPort = 6001;
    std::string serverHost = "127.0.0.1";
    std::string path = "test";

    std::string requrestURI = "coap://" + serverHost + ":" + std::to_string(serverPort) + "/" + path;

    libCoapServer = new LibCoapServer(serverHost.c_str(), serverPort);

    std::string payload = "testing";

    class postEndpoint : public LibCoapEndpoint
    {
    public:
        LibCoapServer * server;
        int callbackCount;
        int expectedRequestType;
        unsigned char * expectedPayload;
        unsigned short expectedPayloadLength;

        postEndpoint(LibCoapServer * server, coap_context_t * context, const unsigned char * path, int expectedRequestType, unsigned char * expectedPayload, unsigned short expectedPayloadLength)
        : LibCoapEndpoint(context, path), server(server), callbackCount(0), expectedRequestType(expectedRequestType), expectedPayload(expectedPayload) , expectedPayloadLength(expectedPayloadLength) {}

        virtual void HandleRequest(coap_context_t * ctx,
                struct coap_resource_t * resource,
                const coap_endpoint_t * local_interface,
                coap_address_t * peer,
                coap_pdu_t * request,
                str * token,
                coap_pdu_t * response)
        {
            size_t requestLength;
            unsigned char * requestData;
            unsigned char optbuf[2];

            EXPECT_EQ(expectedRequestType, GetRequestType(request));

            if(expectedPayload != NULL)
            {
                EXPECT_TRUE(coap_get_data(request, &requestLength, &requestData) != 0);
                EXPECT_EQ(expectedPayloadLength, requestLength);
                EXPECT_EQ(0, memcmp(requestData, expectedPayload, requestLength));
            }

            coap_add_option(response, COAP_OPTION_CONTENT_TYPE,
                    coap_encode_var_bytes(optbuf, GetRequestType(request)), optbuf);

            response->hdr->code = COAP_RESPONSE_CODE(200);

            callbackCount++;
            server->running = false;
        };

    };
    postEndpoint endpoint(libCoapServer, libCoapServer->libcoapContext, (const unsigned char *)path.c_str(), COAP_REQUEST_POST, (unsigned char *)payload.c_str(), payload.length());

    coap_PostRequest(NULL, requrestURI.c_str(), ContentType_ApplicationPlainText, (const char *)payload.c_str(), payload.length(), ClientTransactionCallback);

    EXPECT_TRUE(libCoapServer->CoapHandleMessages(5));
    EXPECT_EQ(1, endpoint.callbackCount);

    coap_HandleMessage();

    coap_Destroy();

    delete libCoapServer;
}
