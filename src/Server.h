#pragma once
#include<WinSock2.h>
#include<iostream>
#include<thread>

class Server
{
public:
	Server(const int& port);
	~Server();


private:
	SOCKET* _serverSocket;

	void handleClient(const SOCKET& clientSocket);
};

