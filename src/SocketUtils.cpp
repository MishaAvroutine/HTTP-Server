#include "SocketUtils.h"

void SocketUtils::sendToClient(const std::string& response, const SOCKET& socket)
{
	try
	{
		send(socket,response.c_str(), response.size() + 1, 0);
	}
	catch (...)
	{
		throw std::exception(__FUNCTION__ " - Failed to send response");
	}
}

std::string SocketUtils::recvFromClient(const SOCKET& socket)
{
	char buf[KB* 4];
	int recived = recv(socket, buf, KB * 4, 0);
	buf[recived] = '\0';
	return std::string(buf);
}
