/*

 Copyright (c) 2023 Oliver Lau <oliver.lau@gmail.com>

 Permission is hereby granted, free of charge, to any person
 obtaining a copy of this software and associated documentation
 files (the “Software”), to deal in the Software without
 restriction, including without limitation the rights to use,
 copy, modify, merge, publish, distribute, sublicense, and/or
 sell copies of the Software, and to permit persons to whom the
 Software is furnished to do so, subject to the following
 conditions:

 The above copyright notice and this permission notice shall be
 included in all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND,
 EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE
 AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 OTHER DEALINGS IN THE SOFTWARE.

*/

#include <cstdint>
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
                result << "\\" << std::setw(3) << std::oct << std::setfill('0') << static_cast<unsigned int>((uint8_t)c);
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
