#pragma once
#include<string>
#include<WinSock2.h>

class SocketUtils
{
public:
	static void sendToClient(const std::string& response,const SOCKET& socket);
};

