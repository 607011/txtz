
#include <iomanip>
#include <iostream>
#include <sstream>

#include "util.hpp"

namespace util
{
    std::string escaped(std::string const &str)
    {
        std::ostringstream result;
        for (auto it = std::begin(str); it != std::end(str); ++it)
        {
            char c = *it;
            if (c == '"' || c == '\\')
            {
                result << '\\' << c;
            }
            else if (' ' <= c && c <= '~')
            {
                result << c;
            }
            else
            {
                result << "\\x" << std::setw(2) << std::hex << std::setfill('0') << static_cast<unsigned int>((uint8_t)c);
            }
        }
        return result.str();
    }

    std::pair<std::string, std::string> unpair(const std::string &str, char delim)
    {
        std::pair<std::string, std::string> pair;
        auto index = str.find(delim);
        if (index != std::string::npos)
        {
            pair = std::make_pair(str.substr(0, index),
                                  str.substr(index + 1, std::string::npos));
        }
        else
        {
            pair = std::make_pair(str, std::string());
        }
        return pair;
    }

    std::vector<std::string> split(const std::string &str, char delim)
    {
        std::vector<std::string> strings;
        std::size_t start;
        std::size_t end = 0;
        while ((start = str.find_first_not_of(delim, end)) != std::string::npos)
        {
            end = str.find(delim, start);
            strings.push_back(str.substr(start, end - start));
        }
        return strings;
    }

}