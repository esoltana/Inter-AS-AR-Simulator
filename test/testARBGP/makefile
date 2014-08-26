OBJS = ARBGP.o IPCE.o intradijkstra.o
CC = g++
DEBUG = -g
CFLAGS = -Wall -c $(DEBUG)
LFLAGS = -Wall $(DEBUG)

test_ARBGP : $(OBJS)
	$(CC) $(LFLAGS) testARBGP.cpp $(OBJS) -o test_ARBGP

ARBGP.o : ../../src/ARBGP.cpp ../../src/ARBGP.h
	$(CC) $(CFLAGS) ../../src/ARBGP.cpp
	
IPCE.o : ../../src/IPCE.cpp ../../src/IPCE.h intradijkstra.o
	$(CC) $(CFLAGS) ../../src/IPCE.cpp intradijkstra.o
	
intradijkstra.o : ../../src/intradijkstra.cpp ../../src/intradijkstra.h
	$(CC) $(CFLAGS) ../../src/intradijkstra.cpp
	
clean:
	\rm *.o *~ test_ARBGP
