echo 'gcc -c -std=c++14 -I/usr/include/modern/include/ -D_RED_PITAYA_HW -I/opt/redpitaya/include/ -O0 -g -fconcepts -DVERSION= -DREVISION=  acq_main.cpp -o acq_main.o'
gcc -c -std=c++14 -I/usr/include/modern/include/ -D_RED_PITAYA_HW -I/opt/redpitaya/include/ -O0 -g -fconcepts -DVERSION= -DREVISION=  acq_main.cpp -o acq_main.o

echo 'gcc -c -std=c++14 -I/usr/include/modern/include/ -D_RED_PITAYA_HW -I/opt/redpitaya/include/ -O0 -g -fconcepts -DVERSION= -DREVISION=  rfacq.cpp -o rfacq.o'
gcc -c -std=c++14 -I/usr/include/modern/include/ -D_RED_PITAYA_HW -I/opt/redpitaya/include/ -O0 -g -fconcepts -DVERSION= -DREVISION=  rfacq.cpp -o rfacq.o

echo 'gcc -g acq_main.o rfacq.o -pthread -ljsoncpp -lzmq -lstdc++ -ljsoncpp  -L/opt/redpitaya/lib -lrp -o acq_main'
gcc -g acq_main.o rfacq.o -pthread -ljsoncpp -lzmq -lstdc++ -ljsoncpp  -L/opt/redpitaya/lib -lrp -o acq_main
