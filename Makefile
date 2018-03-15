CC=g++
CSTD=-std=c++14
CFLAGS=-Wall -pedantic
SERVERINC=-Iinclude
CLIENTINC=-Isha256 -Iinclude
SERVERSRC=src/Server.cpp
CLIENTSRC=sha256/sha256.cpp src/Client.cpp
SERVEREXEC=bin/Server
CLIENTEXEC=bin/Client

all: clean build_server_exec build_client_exec

build_server_exec:
	$(CC) $(CSTD) $(CFLAGS) $(SERVERINC) $(SERVERSRC) -o $(SERVEREXEC) -lzmq
	
build_client_exec:
	$(CC) $(CSTD) $(CFLAGS) $(CLIENTINC) $(CLIENTSRC) -o $(CLIENTEXEC) -lzmq

clean:
	rm -f bin/*
