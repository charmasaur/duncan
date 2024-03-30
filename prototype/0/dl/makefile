CC = g++ -g -Wall
LDFLAGS = 
RM = rm -f

downloader.o : downloadlistener.h downloader.h downloader.cpp
	$(CC) downloader.cpp -c -o downloader.o

clean :
	$(RM) *.o
