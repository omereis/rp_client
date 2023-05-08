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
ifeq ($(TARGET),$(DEFAULT_TARGET))
	CPP_DEF = '_RED_PITAYA_HW'
	CFLAGS = -std=c++14 -g -I/usr/include/modern/include/ -D_RED_PITAYA_HW
else
	CPP_DEF = 'docker'
endif

LDFLAGS = -pthread -ljsoncpp -lzmq -lstdc++ -ljsoncpp -lm

RP_OBJ = rp_server.o rp_setup.o trim.o misc.o mca_params.o pulse_info.o RedPitayaTrigger.o RedPitayaSampling.o pulse_index.o TrpzInfo.o PulseFilterInfo.o remote_proc.o
RP_CLIENTS = rp_client.o
#RP_SRC = rp_server.cpp rp_setup.cpp trim.cpp misc.cpp mca_params.cpp pulse_info.cpp
RP_INC = rp_setup.h

all: rp_server 

.cpp.o: $(RP_INC) docker.mak
	$(CPP) -c $(CFLAGS) $< -o $@

rp_server: $(RP_OBJ)
	$(CPP) -g $(RP_OBJ) $(LDFLAGS) -o $@

clean:
	rm *.o rp_server 


