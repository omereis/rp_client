/*****************************************************************************\
| zmq_client.cpp                                                              |
\*****************************************************************************/

//  Hello World client
#include <zmq.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>

int main (void)
{
    printf ("Connecting to hello world server…\n");
    void *context = zmq_ctx_new ();
    void *requester = zmq_socket (context, ZMQ_REQ);
    zmq_connect (requester, "tcp://127.0.0.1:5009");
    //zmq_connect (requester, "tcp://localhost:5555");

    int request_nbr;
    for (request_nbr = 0; request_nbr != 10; request_nbr++) {
        char buffer [10];
        printf ("Sending Hello %d…\n", request_nbr);
        int nSend = zmq_send (requester, "Hello", 5, 0);
		fprintf (stderr, "%d sent\n", nSend);
        zmq_recv (requester, buffer, 10, 0);
        printf ("Received World %d\n", request_nbr);
    }
    zmq_close (requester);
    zmq_ctx_destroy (context);
    return 0;
}
/*
//  Hello World server

#include <zmqpp/zmqpp.hpp>
#include <string>
#include <iostream>
#include <chrono>
#include <thread>

using namespace std;

int main(int argc, char *argv[]) {
  const string endpoint = "tcp://*:5555";

  // initialize the 0MQ context
  zmqpp::context context;

  // generate a pull socket
  zmqpp::socket_type type = zmqpp::socket_type::reply;
  zmqpp::socket socket (context, type);

  // bind to the socket
  socket.bind(endpoint);
  while (1) {
    // receive the message
    zmqpp::message message;
    // decompose the message 
    socket.receive(message);
    string text;
    message >> text;

    //Do some 'work'
    std::this_thread::sleep_for(std::chrono::seconds(1));
    cout << "Received Hello" << endl;
    socket.send("World");
  }

}
*/

