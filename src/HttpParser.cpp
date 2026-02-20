#include "HttpParser.h"

HttpParser::HttpParser(const std::string& request)
{
	parseHttpRequest(request);
}

std::string& HttpParser::operator[](const std::string& key)
{
	try
	{
		return _headers.at(key);
	}
	catch (...)
	{
		return _method;
	}
}

std::string HttpParser::getMethod() const
{
	return _method;
}

std::string HttpParser::getPath() const
{
	return _path;
}

std::string HttpParser::getContents() const
{
	return _content;
}

std::string HttpParser::getHttpVersion() const
{
	return _httpVersion;
}

void HttpParser::parseHttpRequest(const std::string& request)
{
	{
		std::istringstream iss(request);
		iss >> _method;
		iss >> _path;
		iss >> _httpVersion;
	}
	std::stringstream oss(request);
	std::string token;
	while (std::getline(oss,token))
	{
		if (token != "\r\n")
		{

			std::vector<std::string> headerAndValue = StringUtils::split(token, ':');
			if (headerAndValue.size() != 2)
				throw std::exception("Unable To Parse Http request " __FUNCTION__);
			_headers[headerAndValue[0]] = headerAndValue[1];
		}
		else
		{
			break;
		}
	}

	while (std::getline(oss, token))
	{
		_content += token;
	}
}