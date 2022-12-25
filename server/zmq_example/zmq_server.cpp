/*****************************************************************************\
| zmq_server.cpp                                                              |
\*****************************************************************************/
#include <zmq.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <assert.h>

int main (void)
{
    //  Socket to talk to clients
    void *context = zmq_ctx_new ();
    void *responder = zmq_socket (context, ZMQ_REP);
    //int rc = zmq_bind (responder, "tcp://*:5555");
    int rc = zmq_bind (responder, "tcp://*:5009");
    assert (rc == 0);

    while (1) {
        char buffer [10];
		fprintf (stderr, "Waiting...");
        zmq_recv (responder, buffer, 10, 0);
        printf ("Received Hello\n");
        sleep (1);          //  Do some 'work'
        zmq_send (responder, "World", 5, 0);
    }
    return 0;
}

/*
//  Hello World client
#include <zmqpp/zmqpp.hpp>
#include <string>
#include <iostream>

using namespace std;

int main(int argc, char *argv[]) {
  const string endpoint = "tcp://localhost:5555";

  // initialize the 0MQ context
  zmqpp::context context;

  // generate a push socket
  zmqpp::socket_type type = zmqpp::socket_type::req;
  zmqpp::socket socket (context, type);

  // open the connection
  cout << "Connecting to hello world server…" << endl;
  socket.connect(endpoint);
  int request_nbr;
  for (request_nbr = 0; request_nbr != 10; request_nbr++) {
    // send a message
    cout << "Sending Hello " << request_nbr <<"…" << endl;
    zmqpp::message message;
    // compose a message from a string and a number
    message << "Hello";
    socket.send(message);
    string buffer;
    socket.receive(buffer);
    
    cout << "Received World " << request_nbr << endl;
  }
}
*/


//  Hello World server
