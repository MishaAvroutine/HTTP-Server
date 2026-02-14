#pragma once
#include<string>

class HttpResponse
{
public:
	HttpResponse(const std::string& msg);
	std::string getHeaders() const;
	virtual ~HttpResponse();
	std::string getContentType(const std::string& fileName) const;

private:
	std::string _response;
};

