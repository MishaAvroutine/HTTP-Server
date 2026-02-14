#include"HttpResponse.h"

HttpResponse::HttpResponse(const std::string& msg): _response(msg)
{
}

std::string HttpResponse::getHeaders() const
{
	return _response;

}

HttpResponse::~HttpResponse()
{
}

std::string HttpResponse::getContentType(const std::string& fileName) const
{
	if (fileName.ends_with(".html")) return "text/html";
	if (fileName.ends_with(".css")) return "text/css";
	if (fileName.ends_with(".js")) return "application/javascript";
	if (fileName.ends_with(".png")) return "image/png";
	if (fileName.ends_with(".jpg")) return "image/jpeg";
	return "text/plain";
}
