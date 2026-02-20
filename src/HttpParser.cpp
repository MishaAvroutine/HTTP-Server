#include "HttpParser.h"

HttpParser::HttpParser(const std::string& request)
{
	parseHttpRequest(request);
}

const std::string* HttpParser::getHeader(const std::string& key) const
{
	auto it = _headers.find(key);
	if (it == _headers.end())
		return nullptr;

	return &it->second;
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

std::unique_ptr<HttpResponse> HttpParser::handleRequest()
{
	std::unique_ptr<HttpResponse> response;
    if (_method == "GET")
    {
		response.reset(handleGETRequest());
    }
	return response;
}

void HttpParser::parseHttpRequest(const std::string& request)
{
    std::istringstream stream(request);
    std::string line;

    if (!std::getline(stream, line))
        throw std::runtime_error("Empty request");

    std::istringstream firstLine(line);
    if (!(firstLine >> _method >> _path >> _httpVersion))
        throw std::runtime_error("Invalid request line");

    // Headers
    while (std::getline(stream, line))
    {
        if (line == "\r" || line.empty())
            break;

        if (line.back() == '\r')
            line.pop_back();

        auto pos = line.find(':');
        if (pos == std::string::npos)
            throw std::runtime_error("Invalid header");

        std::string key = line.substr(0, pos);
        std::string value = line.substr(pos + 1);

        key = StringUtils::trim(key);
        value = StringUtils::trim(value);

        std::transform(key.begin(), key.end(), key.begin(), ::tolower);

        _headers[key] = value;
    }

    // Body (basic version)
    std::ostringstream body;
    body << stream.rdbuf();
    _content = body.str();
}








/*
function to handle the HTTP GET request
input: the request
ouput: the ready packet after parsing
*/
HttpResponse* HttpParser::handleGETRequest()
{

	if (_path.empty() || _path == "/")
		_path = INDEX_FILE;

	if (_path.front() == '/')
		_path.erase(0, 1);

	if (_path.find("..") != std::string::npos)
		return new  HttpResponse403Forbidden();

	return getResponse();
}


/*
function to ge the response after parsing
input: filename
output: http mime header/data
*/
HttpResponse* HttpParser::getResponse()
{
	std::string content = readFile(_path);
	return responseCode(content);
}


/*
function to get the httpResponse code packet
input: contents of the file and it's filename
output: the packet with the response ready including headers and the contents
*/
HttpResponse* HttpParser::responseCode(const std::string & contents)
{
	HttpResponse* response = nullptr;
	switch (contents.size())
	{
	case 0:
		response = new HttpResponseNotFound404();
		break;
	default:
		response = new HttpReponseOk200(contents, _path);
		break;
	}
	return response;
}


/*
function to read the file with the filename
input: std::string filename
output: std::string contaning file contents
*/
std::string HttpParser::readFile(const std::string& fileName)
{
	std::ifstream file("../ServerFiles/" + fileName, std::ios::binary);
	if (!file.is_open())
		return "";

	std::ostringstream ss;
	ss << file.rdbuf();
	return ss.str();
}