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
#CFLAGS = -std=c++14 -g -I/usr/include/modern/include/
#CFLAGS = -std=c++14 -g -I/usr/include/modern/include/ -D_RED_PITAYA_HW -I/opt/redpitaya/include/
CFLAGS = -std=c++14 -I/usr/include/modern/include/ -D_RED_PITAYA_HW -I/opt/redpitaya/include/ -O0 -g -fconcepts
#CFLAGS +=  -std=c++14
CFLAGS += -DVERSION=$(VERSION) -DREVISION=$(REVISION) 


OBJECTS = read_json.o rp_setup.o trim.o misc.o mca_params.o RedPitayaTrigger.o RedPitayaSampling.o
INCLUDES = rp_setup.h misc.h timer.h pulse_info.h RedPitayaTrigger.h RedPitayaSampling.h bd_types.h
SOURCES = read_json.cpp rp_setup.cpp trim.cpp misc.cpp RedPitayaTrigger.cpp RedPitayaSampling.cpp
LDFLAGS = -pthread -ljsoncpp -lzmq -lstdc++ -ljsoncpp  -L/opt/redpitaya/lib -lrp

RP_OBJ = rp_server.o rp_setup.o trim.o misc.o mca_params.o calc_mca.o pulse_info.o RedPitayaTrigger.o RedPitayaSampling.o
RP_CLIENTS = rp_client.o
RP_SRC = rp_server.cpp rp_setup.cpp trim.cpp misc.cpp mca_params.cpp calc_mca.cpp pulse_info.cpp
RP_INC = 

#all: read_json rp_server rp_client

.cpp.o: docker.mak
	$(CPP) -c $(CFLAGS) $< -o $@

rp_server: $(RP_OBJ)
	$(CPP) -g $(RP_OBJ) $(LDFLAGS) -o $@

#rp_client: $(RP_CLIENTS)
	#$(CPP) -g $(RP_OBJ) $(LDFLAGS) -o $@

#read_json: $(OBJECTS)
	#$(CPP) -g $(OBJECTS) $(LDFLAGS) -o $@

clean:
	rm *.o rp_client rp_server read_json

clean_all:
	rm *.o rp_client rp_server read_json

