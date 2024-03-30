CC = g++ -g -Wall
LDFLAGS =
RM = rm -f

hitb : hitb.cpp ../timer/timer.o
	$(CC) hitb.cpp ../timer/timer.o $(LDFLAGS) -o hitb

clean :
	$(RM) hitb *.o
