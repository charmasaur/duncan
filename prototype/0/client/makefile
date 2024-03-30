CC = g++ -g -Wall 
LDFLAGS = -lX11 -lncurses -lenet -lcurlpp -lcurl
RM = rm -f

duncan : duncan.cpp col.o client.o input.o startstate.o gamestate.o inputline.o ncwindow.o world.o map.o character.o pathfinder.o ../network/connection.o ../dl/downloader.o ../map/map.o
	$(CC) duncan.cpp col.o client.o input.o startstate.o gamestate.o inputline.o ncwindow.o world.o map.o character.o pathfinder.o ../network/connection.o ../dl/downloader.o ../map/map.o -o duncan $(LDFLAGS)

client.o : client.h client.cpp
	$(CC) client.cpp -c -o client.o
input.o : input.h input.cpp
	$(CC) input.cpp -c -o input.o 
startstate.o : startstate.h startstate.cpp
	$(CC) startstate.cpp -c -o startstate.o
gamestate.o : gamestate.h gamestate.cpp
	$(CC) gamestate.cpp -c -o gamestate.o
inputline.o : inputline.h inputline.cpp
	$(CC) inputline.cpp -c -o inputline.o
ncwindow.o : ncwindow.h ncwindow.cpp
	$(CC) ncwindow.cpp -c -o ncwindow.o
world.o : world.h world.cpp
	$(CC) world.cpp -c -o world.o
map.o : map.h map.cpp
	$(CC) map.cpp -c -o map.o
character.o : character.h character.cpp
	$(CC) character.cpp -c -o character.o
pathfinder.o : pathfinder.h pathfinder.cpp
	$(CC) pathfinder.cpp -c -o pathfinder.o

clean :
	$(RM) duncan *.o
