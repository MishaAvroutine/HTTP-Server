#include "StringUtils.h"

std::vector<std::string> StringUtils::split(const std::string& str, const char& delim)
{
    std::vector<std::string> res;

    std::stringstream oss(str);
    std::string token;
    while (std::getline(oss, token, delim))
    {
        res.push_back(token);
    }

    return res;
}

std::string StringUtils::ltrim(const std::string& s)
{
    size_t start = 0;
    while (start < s.size() && std::isspace(static_cast<unsigned char>(s[start])))
        ++start;

    return s.substr(start);
}

std::string StringUtils::rtrim(const std::string& s)
{
    if (s.empty())
        return s;

    size_t end = s.size() - 1;
    while (end > 0 && std::isspace(static_cast<unsigned char>(s[end])))
        --end;

    return s.substr(0, end + 1);
}

std::string StringUtils::trim(const std::string& s)
{
    return rtrim(ltrim(s));
}