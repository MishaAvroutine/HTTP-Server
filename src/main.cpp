#pragma comment(lib, "Ws2_32.lib")
#include"Server.h"

#define PORT_IDX 1


int main(int argc,const char** argv)
{
    if (argc != 2)
    {
        std::cerr << "Usage: HttpServer <port>" << std::endl;
        return 1;
    }

    // init wsa
    WSADATA wsaData;
    WORD wVersionRequested = MAKEWORD(2, 2);

    int err = WSAStartup(wVersionRequested, &wsaData);
    if (err != 0) {
        std::cerr << "WSAStartup failed with error: " << err << std::endl;
        return 1;
    }
    if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2) {
        std::cerr << "Could not find a usable version of Winsock. Closing..." << std::endl;
        return 1;
    }

    try
    {
        int port = std::stoi(argv[PORT_IDX]);
        Server server(port);
        server.bindServer();

    }
    catch (const std::exception& exp)
    {
        std::cerr << exp.what() << std::endl;
    }
    return 0;
}
