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

//-----------------------------------------------------------------------------
std::string HandleSetup();
//-----------------------------------------------------------------------------
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
        	printf ("Message parsed\n");
            if (!root["setup"].isNull())
                strReply = HandleSetup(root["setup"], rp_setup);
/*
            Json::Value jSetup = root["setup"];
			if (!jSetup.isNull())
	        	printf ("found setup message\n");
			else
	        	printf ("NOT setup message\n");
            if (!jSetup.isNull()) {
	        	printf ("\nParsing setup message\n");
				if (jSetup.isObject()) {
                    rp_setup.UpdateFromJson(jSetup);
                    rp_setup.SaveToJson("rp_setup.json");
				}
				else {
                	string strCmd = jSetup.asString();
					printf ("Command parsed: '%s'\n", strCmd.c_str());
                	if (ToLower(strCmd) == string("read")) {
                    	strReply = StringifyJson (rp_setup.AsJson());
                	}
				}
            }
            printf ("Message: %s\n", StringifyJson(root).c_str());
*/
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
std::string HandleSetup(Json::Value &jSetup, TRedPitayaSetup &rp_setup)
{
    std::string strReply;

    try {
    	if (jSetup.isObject()) {
            rp_setup.UpdateFromJson(jSetup);
            rp_setup.SaveToJson("rp_setup.json");
		}
        strReply = StringifyJson (rp_setup.AsJson());
    }
    catch (std::exception &err) {
        Json::Value root;

        root["error"] = err.what();
        strReply = StringifyJson (root);
    }
    return (strReply);
}
//-----------------------------------------------------------------------------

