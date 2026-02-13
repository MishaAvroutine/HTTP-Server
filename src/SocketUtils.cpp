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
