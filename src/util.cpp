
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

}