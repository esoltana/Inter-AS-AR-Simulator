OBJS = CallGenerator.o
CC = g++
DEBUG = -g
CFLAGS = -Wall -c $(DEBUG)
LFLAGS = -Wall $(DEBUG)

test_CallGenerator : $(OBJS)
	$(CC) $(LFLAGS) testCallGenerator.cpp ../src/randgen.c $(OBJS) -o test_CallGenerator

CallGenerator.o : ../src/CallGenerator.cpp ../src/CallGenerator.h ../src/randgen.c ../src/randgen.h
	$(CC) $(CFLAGS) ../src/CallGenerator.cpp ../src/randgen.c
	
clean:
	\rm *.o *~ test_CallGenerator
