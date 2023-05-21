#pragma once

#include <string>
#include <sstream>
#include <utility>
#include <vector>

namespace util
{
    std::string escaped(std::string const &str);
    std::vector<std::string> split(const std::string &str, char delim);
    std::pair<std::string, std::string> unpair(const std::string &str, char delim);

    template <typename InputIteratorT, typename SeparatorT>
    std::string join(InputIteratorT input, SeparatorT separator)
    {
        std::ostringstream result;
        auto i = std::begin(input);
        if (i != std::end(input))
        {
            result << *i++;
        }
        while (i != std::end(input))
        {
            result << separator << *i++;
        }
        return result.str();
    }

}
