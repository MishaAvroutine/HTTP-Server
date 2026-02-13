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
		throw std::exception(__FUNCTION__ " - faild to close socket");
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

void Server::handleClient(const SOCKET& clientSocket)
{
	try
	{
		// since Http is a stateless protocol, no loops needed, HTTP GET -> response ->  disconnect
		char buf[KB * 4];
		int read = recv(clientSocket, buf, KB * 4, 0);
		buf[read] = '\0';

		std::string headers(buf);
	
		std::string response;
		if (headers.find("GET /") == 0)
		{
			response = handleGETRequest(headers);
		}

		SocketUtils::sendToClient(response, clientSocket);

		disconnectClient(clientSocket);
	}
	catch (const std::exception& exp)
	{
		std::cerr << exp.what() << std::endl;
	}
}

void Server::disconnectClient(const SOCKET& socket)
{
	connectedClients--;
	std::cout << "Client Disconnected: " << socket << std::endl;
	closesocket(socket);
}

std::string Server::handleGETRequest(std::string& request)
{
	/*std::stringstream requestStream(request);
	std::string line;
	std::getline(requestStream,line);
	size_t it = line.find(" HTTP/1.1");
	if (it == std::string::npos)
		throw std::exception(__FUNCTION__ " - invalid http GET");
	std::string fileName = line.substr(GET_PREFIX_LEN, it - GET_PREFIX_LEN);*/

	std::stringstream oss;

	oss << "HTTP/1.1 200 OK\r\n";
	oss << "Content-Type: text/plain\r\n";
	oss << "Content-Length: 5\r\n";
	oss << "Connection: keep-alive\r\n";
	oss << "\r\n";
	oss << "Hello";

	return oss.str();
}
