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
CPP_DEF = 'docker'
TARGET = 'docker'
DEFAULT_TARGET='RED_PITAYA_HW'

OS = 'linux'
myOS = 'docker'

ifeq ($(OS),$(myOS))
	message = 'Linux'
else
	message = 'Docker'
endif

BASIC_CFLAGS = -std=c++14 -g -I/usr/include/modern/include/
CFLAGS = $(BASIC_CFLAGS)
#ifeq ($(TARGET),'RED_PITAYA_HW'))
ifeq ($(TARGET),$(DEFAULT_TARGET))
	CPP_DEF = '_RED_PITAYA_HW'
	CFLAGS = -std=c++14 -g -I/usr/include/modern/include/ -D_RED_PITAYA_HW
else
	CPP_DEF = 'docker'
	#CFLAGS = $(BASIC_CFLAGS)
endif

OBJECTS = read_json.o rp_setup.o trim.o misc.o mca_params.o RedPitayaTrigger.o RedPitayaSampling.o
INCLUDES = rp_setup.h misc.h timer.h pulse_info.h RedPitayaTrigger.h RedPitayaSampling.h
SOURCES = read_json.cpp rp_setup.cpp trim.cpp misc.cpp RedPitayaTrigger.cpp RedPitayaSampling.cpp
LDFLAGS = -pthread -ljsoncpp -lzmq -lstdc++ -ljsoncpp

RP_OBJ = rp_server.o rp_setup.o trim.o misc.o mca_params.o calc_mca.o pulse_info.o RedPitayaTrigger.o RedPitayaSampling.o
RP_CLIENTS = rp_client.o
RP_SRC = rp_server.cpp rp_setup.cpp trim.cpp misc.cpp mca_params.cpp calc_mca.cpp pulse_info.cpp
RP_INC = 

all: read_json rp_server rp_client

.cpp.o:
	$(CPP) -c $(CFLAGS) $< -o $@

rp_server: $(RP_OBJ)
	$(CPP) -g $(RP_OBJ) $(LDFLAGS) -o $@

rp_client: $(RP_CLIENTS)
	$(CPP) -g $(RP_OBJ) $(LDFLAGS) -o $@

read_json: $(OBJECTS)
	$(CPP) -g $(OBJECTS) $(LDFLAGS) -o $@

clean:
	rm *.o rp_client rp_server read_json

clean_all:
	rm *.o rp_client rp_server read_json

