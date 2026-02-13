#pragma comment(lib, "Ws2_32.lib")
#include"Server.h"

int main(int argc,const char** argv)
{
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


}
