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
CFLAGS = -std=c++14 -g

OBJECTS = read_json.o rp_setup.o trim.o misc.o mca_params.o
INCLUDES = rp_setup.h misc.h timer.h
SOURCES = read_json.cpp rp_setup.cpp trim.cpp misc.cpp
LDFLAGS = -pthread -ljsoncpp -lzmq -lstdc++ -ljsoncpp

RP_OBJ = rp_server.o rp_setup.o trim.o misc.o mca_params.o calc_mca.o
RP_SRC = rp_server.cpp rp_setup.cpp trim.cpp misc.cpp mca_params.cpp calc_mca.cpp
RP_INC = 

all: read_json rp_server

.cpp.o:
	$(CPP) -c $(CFLAGS) $< -o $@

rp_server: $(RP_OBJ)
	$(CPP) -g $(RP_OBJ) $(LDFLAGS) -o $@

read_json: $(OBJECTS)
	$(CPP) -g $(OBJECTS) $(LDFLAGS) -o $@

clean:
	rm *.o

clean_all:
	rm *.o rp_client rp_server

