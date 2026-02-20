#include"Server.h"

Server::Server(const unsigned int& port) : _port(port) , connectedClients(0)
{
	_serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (_serverSocket == INVALID_SOCKET)
	{
		throw std::exception(__FUNCTION__ " - failed to create socket");
	}
}

Server::~Server()
{
	try
	{
		closesocket(_serverSocket);
	}
	catch (...)
	{
		throw std::exception(__FUNCTION__ " - failed to close socket");
	}
}

void Server::bindServer()
{
	struct sockaddr_in sa = { 0 };

	sa.sin_port = htons(_port);
	sa.sin_family = AF_INET;
	sa.sin_addr.s_addr = INADDR_ANY;

	if (bind(_serverSocket, (struct sockaddr*)&sa, sizeof(sa)) == SOCKET_ERROR)
	{
		throw std::exception(__FUNCTION__ " - bind");
	}
	std::cout << "Binded..." << std::endl;
	if (listen(_serverSocket, SOMAXCONN) == SOCKET_ERROR)
		throw std::exception(__FUNCTION__ " - listen");
	start();
}

/*
the accepting client loop
input: none
output: none
*/
void Server::start()
{
	while (true)
	{
		if (connectedClients > LOAD_BALANCE)
			continue;
		SOCKET client = accept(_serverSocket, NULL, NULL);
		if (client == INVALID_SOCKET)
			throw std::exception(__FUNCTION__ " - accept");
		connectedClients++;
		std::cout << "Client Accepted" << std::endl;

		std::thread t(&Server::handleClient, this, client);
		t.detach();
	}
}

/*
the function that each thread exectutes for each client
input: client socket
ouput: none
*/
void Server::handleClient(const SOCKET& clientSocket)
{
	while (true)
	{
		try
		{
			std::string headers = SocketUtils::recvFromClient(clientSocket);

			if (headers.empty())
				break;

			HttpParser parser(headers);

			std::unique_ptr<HttpResponse> response = parser.handleRequest();

			if (response)
			{
				SocketUtils::sendToClient(response->getHeaders(), clientSocket);
			}

			if (*parser.getHeader("connection") != "keep-alive")
				break;
		}
		catch (const std::exception& exp)
		{
			std::cerr << exp.what() << std::endl;
			break;
		}
	}

	disconnectClient(clientSocket);
}

/*
function to disconnect client from server
*/
void Server::disconnectClient(const SOCKET& socket)
{
	connectedClients--;
	std::cout << "Client Disconnected: " << socket << std::endl;
	closesocket(socket);
}