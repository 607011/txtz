#ifndef __MAPPINGS_HPP__
#define __MAPPINGS_HPP__

#include <string>
#include <unordered_map>

#include "shannon-fano.hpp"

namespace shannon_fano
{
    extern const std::unordered_map<std::string, shannon_fano::code> compression_table;
}

#endif // __MAPPINGS_HPP__
