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
    string strReply;

    Json::Value jSetup = rp_setup.AsJson();
    rp_setup.LoadFromJson("rp_setup.json");
    printf ("Setup: %s\n", StringifyJson(jSetup).c_str());
    while (1) {
        char buffer [1024];
        zmq_recv (responder, buffer, 1024, 0);
        std::string strJson = ToLower (buffer);
        printf ("Received Message:\n%s\n", strJson.c_str());
        if (reader.parse (strJson, root)) {
            Json::Value jSetup = root["setup"];
            if (!jSetup.isNull()) {
                string strCmd = jSetup.asString();
                if (ToLower(strCmd) == string("read")) {
                    Json::Value RpSetup = rp_setup.AsJson();
                    strReply = StringifyJson (RpSetup);
                }
            }

            printf ("Message: %s\n", StringifyJson(root).c_str());
        }
        sleep (1);          //  Do some 'work'
        if (strReply.length() > 0)
            zmq_send (responder, strReply.c_str(), strReply.length(), 0);
        else
            zmq_send (responder, "World", 5, 0);
    }
    return 0;
}
//-----------------------------------------------------------------------------

