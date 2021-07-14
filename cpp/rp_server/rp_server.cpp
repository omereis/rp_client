/******************************************************************************\
|                               rp_server.cpp                                  |
\******************************************************************************/

//  Hello World server
#include <zmq.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <assert.h>

using namespace std;
#include "rp_setup.h"
#include "misc.h"

#include "jsoncpp/json/json.h"

std::string ToLower (const std::string &str);

int main (void)
{
    //  Socket to talk to clients
    void *context = zmq_ctx_new ();
    void *responder = zmq_socket (context, ZMQ_REP);
    int rc = zmq_bind (responder, "tcp://*:5555");
    assert (rc == 0);
	Json::Value root;
	Json::Reader reader;
    TRedPitayaSetup rp_setup;

    Json::Value jSetup = rp_setup.AsJson();
    rp_setup.LoadFromJson("rp_setup.json");
    printf ("Setup: %s\n", StringifyJson(jSetup).c_str());
    while (1) {
        char buffer [1024];
        zmq_recv (responder, buffer, 1024, 0);
        std::string strJson = ToLower (buffer);
        printf ("Received Message:\n%s\n", strJson.c_str());
        sleep (1);          //  Do some 'work'
        zmq_send (responder, "World", 5, 0);
    }
    return 0;
}
//-----------------------------------------------------------------------------

std::string ToLower (const std::string &str)
{
	std::string strLower;

	for (int n=0 ; n < str.size() ; n++)
		strLower += tolower(str[n]);
	return (strLower);
}

