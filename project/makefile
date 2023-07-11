CC = gcc
CLIENT_SOURCES = src/client/client.c src/client/client.h src/general/general.h
SERVER_SOURCES = src/server/server.c src/server/server.h src/server/main.c src/general/general.h

all: client server

client: $(CLIENT_SOURCES)
	$(CC) $(CLIENT_SOURCES) -o client

server: $(SERVER_SOURCES)
	$(CC) $(SERVER_SOURCES) -o server -lm