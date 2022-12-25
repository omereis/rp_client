/*****************************************************************************\
|                                zmq_msg_send.cpp                             |
\*****************************************************************************/
//  Hello World server
#include <zmq.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <assert.h>

int main (void)
{
    int n;
    void *context = zmq_ctx_new ();
    void *requester;

    requester = zmq_socket (context, ZMQ_REQ);
    zmq_connect(requester, "tcp://0.0.0.0:5656");
    //zmq_connect(requester, "tcp://mensadisplay:5556");

    while(1) {
        n = 0;
        double *ad = new double[50];
        zmq_msg_t msg;
        zmq_msg_init_size(&msg, 50 * sizeof (ad[0]));

        for (n=0 ; n < 50 ; n++)
            ad[n] = 100 + (double) n;
        memcpy(zmq_msg_data(&msg), ad, 50 * sizeof (ad[0]));
        zmq_msg_send(&msg, requester, ZMQ_SNDMORE);

        for ( ; n < 100 ; n++)
            ad[n] = 100 + (double) n;
        memcpy(zmq_msg_data(&msg), ad, 50 * sizeof (ad[0]));
        zmq_msg_send(&msg, requester, ZMQ_SNDMORE);

        zmq_msg_init_size(&msg, 0);
        zmq_msg_send(&msg, requester, 0);
        zmq_recv(requester, NULL, 0, 0);
    }

    zmq_close(requester);
    zmq_ctx_destroy(context);
}
