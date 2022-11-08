echo 'gcc -c -std=c++14 -I/usr/include/modern/include/ -D_RED_PITAYA_HW -I/opt/redpitaya/include/ -O0 -g -fconcepts -DVERSION= -DREVISION=  rfacq.cpp -o rfacq.o'
gcc -c -std=c++14 -I/usr/include/modern/include/ -D_RED_PITAYA_HW -I/opt/redpitaya/include/ -O0 -g -fconcepts -DVERSION= -DREVISION=  rfacq.cpp -o rfacq.o

echo 'gcc -g rfacq.o -pthread -ljsoncpp -lzmq -lstdc++ -ljsoncpp  -L/opt/redpitaya/lib -lrp -o rfacq'
gcc -g rfacq.o -pthread -ljsoncpp -lzmq -lstdc++ -ljsoncpp  -L/opt/redpitaya/lib -lrp -o rfacq


