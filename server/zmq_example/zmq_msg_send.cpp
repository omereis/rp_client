/*****************************************************************************\
|                                zmq_msg_send.cpp                             |
\*****************************************************************************/
//  Hello World server
#include <zmq.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>

int main (int argc, char *argv[])
{
    int n, nPort;
    void *context = zmq_ctx_new ();
    void *requester;
	char szRemoteAddress[256], szHost[100];

    requester = zmq_socket (context, ZMQ_REQ);
	if (argc > 1)
		strcpy (szHost, argv[1]);
	else
		strcpy (szHost, "10.0.0.4");
	if (argc > 2)
		nPort = atoi (argv[2]);
	else
		nPort = 5566;
	sprintf (szRemoteAddress, "tcp://%s:%d", szHost, nPort);
	printf ("Remote Host: %s\n", szRemoteAddress);
	//exit (0);
	int nConnect = zmq_connect(requester, szRemoteAddress);
	printf ("Connect status: %d\n", nConnect);
    //zmq_connect(requester, "tcp://0.0.0.0:5656");
    //zmq_connect(requester, "tcp://mensadisplay:5556");
	char szMessage[50];
	strcpy (szMessage, "Next, please");
	nConnect = zmq_send (requester, szMessage, strlen(szMessage), 0);

/*
    while(1) {
        n = 0;
        double *ad = new double[50];
        zmq_msg_t msg;
        zmq_msg_init_size(&msg, 50 * sizeof (ad[0]));

        for (n=0 ; n < 50 ; n++)
            ad[n] = 100 + (double) n;
        memcpy(zmq_msg_data(&msg), ad, 50 * sizeof (ad[0]));
        zmq_msg_send(&msg, requester, 0);
        //zmq_msg_send(&msg, requester, ZMQ_SNDMORE);

        for ( ; n < 100 ; n++)
            ad[n] = 100 + (double) n;
        memcpy(zmq_msg_data(&msg), ad, 50 * sizeof (ad[0]));
        zmq_msg_send(&msg, requester, ZMQ_SNDMORE);

        zmq_msg_init_size(&msg, 0);
        zmq_msg_send(&msg, requester, 0);
        zmq_recv(requester, NULL, 0, 0);
    }
*/

    zmq_close(requester);
    zmq_ctx_destroy(context);
}
