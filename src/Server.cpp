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
		char buf[4 * KB];
		int read = recv(clientSocket, buf, 4 * KB, 0);
		buf[read] = '\0';

		std::string headers(buf);
	
		HttpResponse* response = nullptr;
		if (headers.find("GET /") == 0)
		{
			response = handleGETRequest(headers);
		}
		if(response != nullptr)
			SocketUtils::sendToClient(response->getHeaders(), clientSocket);

		disconnectClient(clientSocket);
		delete response;
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

HttpResponse* Server::handleGETRequest(std::string& request)
{
	std::stringstream requestStream(request);
	std::string line;
	std::getline(requestStream, line);

	size_t it = line.find(" HTTP/1.1");
	if (it == std::string::npos)
		throw std::runtime_error("Invalid HTTP GET");

	std::string fileName = line.substr(GET_PREFIX_LEN, it - GET_PREFIX_LEN);

	if (fileName.empty() || fileName == "/")
		fileName = INDEX_FILE;

	if (fileName.front() == '/')
		fileName.erase(0, 1);

	if (fileName.find("..") != std::string::npos)
		return new HttpResponse("HTTP/1.1 403 Forbidden\r\n\r\n");

	return getResponse(fileName);
}


HttpResponse* Server::getResponse(const std::string& fileName)
{
	std::string content = readFile(fileName);
	return responseCode(content, fileName);
}

HttpResponse* Server::responseCode(const std::string& contents,const std::string& fileName)
{
	HttpResponse* response = nullptr;
	switch (contents.size())
	{
	case 0:
		response = new HttpResponseNotFound404();
		break;
	default:
		response = new HttpReponseOk200(contents,fileName);
		break;
	}
	return response;
}

std::string Server::readFile(const std::string& fileName)
{
	std::ifstream file("../ServerFiles/" + fileName, std::ios::binary);
	if (!file.is_open())
		return "";

	std::ostringstream ss;
	ss << file.rdbuf();
	return ss.str();
}