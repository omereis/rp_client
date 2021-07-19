/******************************************************************************\
|                               rp_server.cpp                                  |
\******************************************************************************/

//  Hello World server
#include <zmq.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <assert.h>

#include <mutex>
#include <thread>

using namespace std;
#include "rp_setup.h"
#include "misc.h"
#include "timer.h"
#include "bd_types.h"

#include "jsoncpp/json/json.h"


TFloatVecQueue g_qPulses;


//-----------------------------------------------------------------------------
std::string HandleSetup(Json::Value &jSetup, TRedPitayaSetup &rp_setup);
std::string HandleRead(Json::Value &jRead, TRedPitayaSetup &rp_setup);
void AddPulse ();
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
    Timer t;

    Json::Value jSetup = rp_setup.AsJson();
    rp_setup.LoadFromJson("rp_setup.json");
    printf ("Setup: %s\n", StringifyJson(jSetup).c_str());
    t.setInterval (AddPulse, 1000);
    while (1) {
        char buffer [1024];
        zmq_recv (responder, buffer, 1024, 0);
        std::string strJson = ToLower (buffer);
        printf ("Received Message:\n%s\n", strJson.c_str());
        if (reader.parse (strJson, root)) {
        	printf ("Message parsed\n");
            if (!root["setup"].isNull())
                strReply = HandleSetup(root["setup"], rp_setup);
            else if (!root["read"].isNull())
                strReply = HandleRead(root["read"], rp_setup);
        }
        sleep (1);          //  Do some 'work'
        if (strReply.length() == 0)
            strReply = std::string("World");
        //if (strReply.length() > 0)
            zmq_send (responder, strReply.c_str(), strReply.length(), 0);
        //else
            //zmq_send (responder, "World", 5, 0);
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

std::string HandleRead(Json::Value &jRead, TRedPitayaSetup &rp_setup)
{
    TFloatVec vPulse;
    TFloatVec::iterator i;
	mutex mtx;
    std::string strReply, strNumber, strPulses;
    char szNum[128];
    Json::Value jPulse(Json::arrayValue);
    int n, nPulses;
    
    try {
        strPulses = jRead["pulse"].asString();
        nPulses = std::stoi(strPulses);
        printf ("Reading pulse");
		if (g_qPulses.size() > 0) {
        	mtx.lock ();
        	vPulse = g_qPulses.back();
        	g_qPulses.pop();
        	mtx.unlock ();
            for (i=vPulse.begin(), n=0 ; (i != vPulse.end()) && (n < nPulses) ; i++, n++) {
                //strNumber = std::to_string (*i);
                sprintf (szNum, "%.3f", *i);
                strNumber = std::string (szNum);
                jPulse.append (strNumber);
            }
        	strReply = StringifyJson(jPulse);
		}
		else
        	strReply = "no pulse";
    }
    catch (std::exception &exp) {
        strReply = std::string("Runtime error in '': ") + std::string (exp.what());
    }
    return (strReply);
}
//-----------------------------------------------------------------------------

void AddPulse ()
{
    TFloatVec vPulse;
    TFloatVec::iterator i;
	mutex mtx;
    static int n=0;
    float fDelta;

    if (ReadVectorFromFile ("pulse.csv", vPulse)) {
        if (n < 100)
            n++;
        else
            n--;
        fDelta = ((float) n) / 100.0;
        for (i=vPulse.begin() ; i != vPulse.end() ; i++)
            *i = *i + fDelta;
        mtx.lock ();
        g_qPulses.push(vPulse);
        while (g_qPulses.size() > 1000)
            g_qPulses.pop();
        mtx.unlock ();
    }
}
//-----------------------------------------------------------------------------

