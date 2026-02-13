#pragma once
#include<WinSock2.h>
#include<iostream>
#include<thread>
#include<string>
#include<sstream>


#include"SocketUtils.h";

#define LOAD_BALANCE 16 // change this according to your power of computing. since i don't wanna blow up my pc
#define KB 1024

#define GET_PREFIX_LEN 5

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


	void disconnectClient(const SOCKET& socket);

	std::string handleGETRequest(std::string& request);
};

