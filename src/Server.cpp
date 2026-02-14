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

			std::unique_ptr<HttpResponse> response;

			std::istringstream iss(headers);
			std::string method;
			iss >> method;

			if (method == "GET")
			{
				response.reset(handleGETRequest(headers));
			}

			if (response)
			{
				SocketUtils::sendToClient(response->getHeaders(), clientSocket);
			}

			if (headers.find("Connection: keep-alive") == std::string::npos)
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


/*
function to handle the HTTP GET request
input: the request
ouput: the ready packet after parsing
*/
HttpResponse* Server::handleGETRequest(const std::string& request)
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
		return new  HttpResponse403Forbidden();

	return getResponse(fileName);
}


/*
function to ge the response after parsing
input: filename
output: http mime header/data
*/
HttpResponse* Server::getResponse(const std::string& fileName)
{
	std::string content = readFile(fileName);
	return responseCode(content, fileName);
}


/*
function to get the httpResponse code packet
input: contents of the file and it's filename
output: the packet with the response ready including headers and the contents
*/
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


/*
function to read the file with the filename
input: std::string filename
output: std::string contaning file contents
*/
std::string Server::readFile(const std::string& fileName)
{
	std::ifstream file("../ServerFiles/" + fileName, std::ios::binary);
	if (!file.is_open())
		return "";

	std::ostringstream ss;
	ss << file.rdbuf();
	return ss.str();
}