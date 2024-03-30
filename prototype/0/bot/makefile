CC = g++ -g -Wall
#LDFLAGS = -Xlinker -Bstatic -lenet -lcurlpp -Xlinker -Bdynamic -lX11 -lncurses -lcurl
LDFLAGS = -lX11 -lncurses -lenet -lcurlpp -lcurl
RM = rm -f

duncanbot : duncanbot.cpp bot.o ../timer/timer.o ../client/input.o ../client/gamestate.o ../client/inputline.o ../client/ncwindow.o ../client/world.o ../client/map.o ../client/character.o ../client/pathfinder.o ../network/connection.o ../dl/downloader.o ../map/map.o
	$(CC) duncanbot.cpp bot.o ../timer/timer.o ../client/input.o ../client/gamestate.o ../client/inputline.o ../client/ncwindow.o ../client/world.o ../client/map.o ../client/character.o ../client/pathfinder.o ../network/connection.o ../dl/downloader.o ../map/map.o -o duncanbot $(LDFLAGS)

bot.o : bot.h bot.cpp
	$(CC) bot.cpp -c -o bot.o

clean :
	$(RM) duncanbot *.o
