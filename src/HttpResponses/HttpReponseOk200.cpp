#include"HttpReponseOk200.h"

HttpReponseOk200::HttpReponseOk200(const std::string& msg,const std::string& fileName) : HttpResponse(
    "HTTP/1.1 200 OK\r\n"
    "Content-Type: " + getContentType(fileName) + "\r\n"
    "Content-Length: " + std::to_string(msg.size()) + "\r\n"
    "Cache-Control: public, max-age=3600\r\n"
    "Connection: close\r\n\r\n" +
    msg
)
{
}
