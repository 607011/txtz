#ifndef __DE_DE_UTF8_HPP__
#define __DE_DE_UTF8_HPP__

#include <string>
#include <unordered_map>

#include "shannon-fano.hpp"

namespace shannon_fano
{
    extern const std::unordered_map<std::string, shannon_fano::code> compression_table;
}
#endif
