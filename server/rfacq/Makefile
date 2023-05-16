#*****************************************************************************\
#                                 Makefile                                    |
#*****************************************************************************/
#
#	Adding source, e.g. file.cpp/h by:
#		add file.o to OBJECTS
#		add file.h to INCLUDES
#		add rule for file.o that depends on file.cpp AND all includes
#*****************************************************************************/
CPP = gcc
#CFLAGS = -std=c++14 -I/usr/include/modern/include/ -D_RED_PITAYA_HW -I/opt/redpitaya/include/ -O0 -g -Wno-psabi
CFLAGS = -std=c++14 -I/usr/include/modern/include/ -D_RED_PITAYA_HW -I/opt/redpitaya/include/ -O0 -g -fconcepts -Wno-psabi
CFLAGS += -DVERSION=$(VERSION) -DREVISION=$(REVISION) 

RP_OBJ = acq_main.o rfacq.o
RP_CLIENTS = rp_client.o
RP_SRC = acq_main.cpp rfacq.cpp
LDFLAGS = -pthread -ljsoncpp -lzmq -lstdc++ -ljsoncpp -lm -L/opt/redpitaya/lib -lrp

.cpp.o: docker.mak
	$(CPP) -c $(CFLAGS) $< -o $@

rfacq: $(RP_OBJ)
	$(CPP) -g $(RP_OBJ) $(LDFLAGS) -o $@

clean:
	rm *.o rfacq

