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
CFLAGS = -std=c++14 -I/usr/include/modern/include/ -D_RED_PITAYA_HW -I/opt/redpitaya/include/ -O0 -fconcepts -Wno-psabi
#CFLAGS = -std=c++14 -I/usr/include/modern/include/ -D_RED_PITAYA_HW -I/opt/redpitaya/include/ -O0 -g -fconcepts -Wno-psabi
CFLAGS += -DVERSION=$(VERSION) -DREVISION=$(REVISION) 

#RP_OBJ = rp_server.o rp_setup.o trim.o misc.o mca_params.o pulse_info.o RedPitayaTrigger.o RedPitayaSampling.o pulse_index.o
RP_OBJ = rp_server.o rp_setup.o trim.o misc.o mca_params.o pulse_info.o RedPitayaTrigger.o RedPitayaSampling.o pulse_index.o TrpzInfo.o PulseFilterInfo.o remote_proc.o sampling.o
RP_CLIENTS = rp_client.o
RP_SRC = rp_server.cpp rp_setup.cpp trim.cpp misc.cpp mca_params.cpp pulse_info.cpp pulse_index.cpp
LDFLAGS = -pthread -ljsoncpp -lzmq -lstdc++ -ljsoncpp -lm -L/opt/redpitaya/lib -lrp

.cpp.o: docker.mak
	$(CPP) -c $(CFLAGS) $< -o $@

rp_server: $(RP_OBJ)
	$(CPP) -g $(RP_OBJ) $(LDFLAGS) -o $@

clean:
	rm *.o rp_server

