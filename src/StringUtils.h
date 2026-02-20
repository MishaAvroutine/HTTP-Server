#pragma once
#include<string>
#include<sstream>
#include<vector>

class StringUtils
{
public:
	static std::vector<std::string> split(const std::string& str, const char& delim);
	static std::string ltrim(const std::string& s);
	static std::string rtrim(const std::string& s);
	static std::string trim(const std::string& s);
};

