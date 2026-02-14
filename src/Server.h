#pragma once
#include<WinSock2.h>
#include<iostream>
#include<thread>
#include<string>
#include<sstream>
#include<fstream>
#include<mutex>
#include"SocketUtils.h"

#include"HttpResponses/HttpResponses.h"

#define LOAD_BALANCE 16 // change this according to your power of computing. since i don't wanna blow up my pc

#define GET_PREFIX_LEN 5

#define INDEX_FILE std::string("index.html")

class Server
{
public:
	Server(const unsigned int& port);
	~Server();

	void bindServer();
	void start();

	static std::string readFile(const std::string& fileName);
private:
	SOCKET _serverSocket;
	unsigned int _port;
	int connectedClients;

	std::mutex mutex_cout;

	void handleClient(const SOCKET& clientSocket);
	void disconnectClient(const SOCKET& socket);
	HttpResponse* handleGETRequest(const std::string& request);
	HttpResponse* getResponse(const std::string& fileName);
	HttpResponse* responseCode(const std::string& contents,const std::string& fileName);
};




