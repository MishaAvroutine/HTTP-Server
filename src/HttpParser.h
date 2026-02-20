#pragma once
#include"StringUtils.h"
#include<unordered_map>
#include<algorithm>
#include"HttpResponses/HttpResponses.h"
#include<sstream>
#include<fstream>


#define GET_PREFIX_LEN 5

#define INDEX_FILE std::string("index.html")

class HttpParser
{
public:
	HttpParser(const std::string& request);

	const std::string* getHeader(const std::string& key) const;
	std::string getMethod() const;
	std::string getPath() const;
	std::string getContents() const;
	std::string getHttpVersion() const;

	std::unique_ptr<HttpResponse> handleRequest();
private:
	std::string _method;
	std::string _path;
	std::string _content;
	std::string _httpVersion;
	std::unordered_map<std::string, std::string> _headers;


	void parseHttpRequest(const std::string& request);


	HttpResponse* handleGETRequest();
	HttpResponse* getResponse();
	HttpResponse* responseCode(const std::string& content);
	static std::string readFile(const std::string& fileName);
};

