CFLAGS = -c -Wall
CC = gcc
CPP = g++
PTLIBS = -pthread
SDLLIBS = -lSDL2 -lSDL2_image

# all: server client

# server: cchess-server.o
# 	${CC} ./object/cchess-server.o -o ./server/server

# cchess-server.o: cchess-server.c
# 	${CC} ${CFLAGS} cchess-server.c ${PTLIBS} -o ./object/cchess-server.o

# client: cchess-client.o
# 	${CC} ./object/cchess-client.o -o ./client/client

# cchess-client.o: cchess-client.c
# 	${CC} ${CFLAGS} cchess-client.c ${PTLIBS} -o ./object/cchess-client.o
all:
	g++ ./client/client.cpp ./object/board.o ./object/texture.o -o ./exec/client -lSDL2 -lSDL2_image

clean:
	rm -f *.o *~