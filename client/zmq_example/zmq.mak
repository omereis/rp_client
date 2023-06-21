#*****************************************************************************\
#                                 Makefile                                    |
#*****************************************************************************/
#

CPP = g++
CFLAGS = -std=c++11 -g -c
LDFLAGS = -lzmq

SEND_OBJS = zmq_msg_send.o
#zmqsend: zmq_msg_send.o

RECV_OBJS = zmq_msg_recv.o
#$(CPP) -g $(SEND_OBJS) $(LDFLAGS) -o zmqrecv

.cpp.o:
	$(CPP) $(CFLAGS)  $< -o $@

#SEND_O: $(SEND_OBJS)
	#$(CPP) -g $(SEND_OBJS) $(LDFLAGS) -o zmqsend

zmqrecv: $(SEND_OBJS) zmq.mak
	$(CPP) -g $(SEND_OBJS) $(LDFLAGS) -o $@

clear:
	rm *.o zmqrecv
