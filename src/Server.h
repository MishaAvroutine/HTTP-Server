#pragma once
#include<WinSock2.h>
#include<iostream>
#include<thread>
#include<string>

#define LOAD_BALANCE 16 // change this according to your power of computing. since i don't wanna blow up my pc


class Server
{
public:
	Server(const unsigned int& port);
	~Server();

	void bindServer();
	void start();

private:
	SOCKET _serverSocket;
	unsigned int _port;
	int connectedClients;

	void handleClient(const SOCKET& clientSocket);
};

