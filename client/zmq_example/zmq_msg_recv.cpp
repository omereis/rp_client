/*****************************************************************************\
|                                zmq_msg_recv.cpp                             |
\*****************************************************************************/
//  Hello World server
#include <zmq.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <assert.h>

void PrintZmqError (int rc);

int main (void)
{
	int64_t more;
	size_t more_size = sizeof more;
	void *context = zmq_ctx_new();
	void *socket = zmq_socket (context, ZMQ_REQ);
	do {
 /* Create an empty ØMQ message to hold the message part */
 		zmq_msg_t part;
 		int rc = zmq_msg_init (&part);
		if (rc != 0)
			PrintZmqError (rc);
 		assert (rc == 0);
 /* Block until a message is available to be received from socket */
 		rc = zmq_msg_recv (&part, socket, 0);
		if (rc != 0)
			PrintZmqError (rc);
 		//assert (rc != -1);
 	/* Determine if more message parts are to follow */
 		rc = zmq_getsockopt (socket, ZMQ_RCVMORE, &more, &more_size);
		if (rc != 0)
			PrintZmqError (rc);
 		//assert (rc == 0);
 		zmq_msg_close (&part);
	} while (more);
	zmq_close (socket);
}

void PrintZmqError (int rc)
{
    if (rc == EAGAIN)
        printf ("Error: EAGAIN\n");
    else if (rc == ENOTSUP)
        printf ("Error: ENOTSUP\n");
    else if (rc == EFSM)
        printf ("Error: EFSM\n");
    else if (rc == ETERM)
        printf ("Error: ETERM\n");
    else if (rc == ENOTSOCK)
        printf ("Error: ENOTSOCK\n");
    else if (rc == EINTR)
        printf ("Error: EINTR\n");
    else if (rc == EFAULT)
        printf ("Error: EFAULT\n");
	else
		printf ("Error %d\n", rc);
}