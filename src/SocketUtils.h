#pragma once
#include<string>
#include<WinSock2.h>
#define KB 1024
class SocketUtils
{
public:
	static void sendToClient(const std::string& response,const SOCKET& socket);
	static std::string recvFromClient(const SOCKET& socket);
};

