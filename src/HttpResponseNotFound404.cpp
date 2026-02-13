#include "HttpResponseNotFound404.h"

HttpResponseNotFound404::HttpResponseNotFound404() : HttpResponse(
	"HTTP/1.1 404 Not Found\r\n"
	"Content-Type: text/html\r\n"
	"Content-Length: 22\r\n"
	"Connection: close\r\n\r\n"
	"<h1>404 Not Found</h1>"
)
{
}
