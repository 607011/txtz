#ifndef __MAPPINGS_HPP__
#define __MAPPINGS_HPP__

#include <string>
#include <unordered_map>

#include "shannon-fano.hpp"

namespace txtz
{
    extern const std::unordered_map<std::string, txtz::code> compression_table;
}

#endif // __MAPPINGS_HPP__
