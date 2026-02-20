#pragma once
#include"StringUtils.h"
#include<unordered_map>

class HttpParser
{
public:
	HttpParser(const std::string& request);


	std::string& operator[](const std::string& key);

	std::string getMethod() const;
	std::string getPath() const;
	std::string getContents() const;
	std::string getHttpVersion() const;

private:
	std::string _method;
	std::string _path;
	std::string _content;
	std::string _httpVersion;
	std::unordered_map<std::string, std::string> _headers;


	void parseHttpRequest(const std::string& request);

};

