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

