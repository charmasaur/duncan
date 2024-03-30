CC = g++ -g -Wall
LDFLAGS = -lenet
RM = rm -f

duncanserver : duncanserver.cpp player.o server.o world.o character.o movable.o npc.o map.o pathfinder.o ../timer/timer.o ../network/connection.o ../map/map.o
	$(CC) duncanserver.cpp player.o server.o world.o character.o movable.o npc.o map.o pathfinder.o ../timer/timer.o ../network/connection.o ../map/map.o -o duncanserver $(LDFLAGS)

npc.o : npc.h npc.cpp
	$(CC) npc.cpp -c -o npc.o
movable.o : movable.h movable.cpp
	$(CC) movable.cpp -c -o movable.o
server.o : server.h server.cpp
	$(CC) server.cpp -c -o server.o
player.o : player.h player.cpp
	$(CC) player.cpp -c -o player.o
world.o : world.h world.cpp charlist.h
	$(CC) world.cpp -c -o world.o
character.o : character.h character.cpp
	$(CC) character.cpp -c -o character.o
map.o : map.h map.cpp
	$(CC) map.cpp -c -o map.o
pathfinder.o : pathfinder.h pathfinder.cpp
	$(CC) pathfinder.cpp -c -o pathfinder.o

clean :
	$(RM) duncanserver *.o
