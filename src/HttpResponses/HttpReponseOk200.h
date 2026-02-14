#pragma once
#include "HttpResponse.h"
class HttpReponseOk200 : public HttpResponse
{
public:
	HttpReponseOk200(const std::string& msg,const std::string& fileName);
};

