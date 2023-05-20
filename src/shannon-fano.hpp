/*
    Copyright (c) 2023 Oliver Lau
*/

#ifndef __SHANNON_FANO_HPP__
#define __SHANNON_FANO_HPP__

#include <bitset>
#include <cstdlib>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>

#include <boost/locale.hpp>

#include "bintree.hpp"

namespace loc = boost::locale;

namespace shannon_fano
{
    typedef uint32_t code_t;

    class code
    {
        uint32_t length_;
        code_t bits_;

    public:
        code()
            : length_(0), bits_(0) {}
        code(uint32_t len, code_t bits)
            : length_(len), bits_(bits) {}
        code(code const &o)
            : length_(o.length_), bits_(o.bits_) {}

        inline void operator=(code const &o)
        {
            length_ = o.length_;
            bits_ = o.bits_;
        }

        inline uint32_t length() const
        {
            return length_;
        }

        inline code_t bits() const
        {
            return bits_;
        }

        inline void append(bool bit)
        {
            bits_ |= code_t(bit) << length_++;
        }

        inline code operator+(bool bit)
        {
            code x{*this};
            x.append(bit);
            return x;
        }

        std::string str() const
        {
            std::ostringstream oss;
            oss << *this;
            return oss.str();
        }

        friend std::ostream &operator<<(std::ostream &os, code const &c)
        {
            code_t b = c.bits_;
            for (uint32_t p = 0; p != c.length_; ++p)
            {
                os << (b & 1);
                b >>= 1;
            }
            return os;
        }
    };

    struct ngram_t
    {
        std::string token{};
        std::size_t weight{0};
        code c;
    };

    void update(std::vector<ngram_t> &p);

    class txtz
    {
        std::unordered_map<std::string, code> compress_table_;
        bintree<code_t, uint32_t, std::string> decompress_tree_;

    public:
        explicit txtz(std::unordered_map<std::string, code> const &table)
            : compress_table_(table)
        {
            for (auto const &it : compress_table_)
            {
                decompress_tree_.append(it.second.bits(), it.second.length(), it.first);
            }
        }
        std::vector<uint8_t> compress(std::string const &str, std::size_t &size)
        {
            std::vector<uint8_t> data;
            namespace blb = boost::locale::boundary;
            loc::generator gen;
            blb::ssegment_index map(blb::character, std::begin(str), std::end(str), gen("de_DE.UTF-8"));
            blb::ssegment_index::iterator e = map.end();
            std::size_t num_bits = 0;
            int bit_idx = 0;
            uint8_t byte = 0;
            data.push_back(static_cast<uint8_t>(str.size()));
            for (blb::ssegment_index::iterator it = map.begin(); it != e; ++it)
            {
                if (compress_table_.find(it->str()) != compress_table_.end())
                {
                    code const &c = compress_table_.at(it->str());
                    code_t value = c.bits() << (8 * sizeof(code_t) - c.length());
                    num_bits += c.length();
                    // XXX: slow bit by bit encoder
                    for (std::size_t i = 0; i < c.length(); ++i)
                    {
                        byte <<= 1;
                        constexpr code_t mask = (1U << (8 * sizeof(code_t) - 1));
                        if ((value & mask) != 0)
                        {
                            byte |= 1;
                        }
                        value <<= 1;
                        if (++bit_idx == 8)
                        {
                            data.push_back(byte);
                            byte = 0;
                            bit_idx = 0;
                        }
                    }
                }
            }
            auto remaining_bits = num_bits - data.size() * 8;
            if (remaining_bits > 0)
            {
                byte <<= 8 - remaining_bits;
                data.push_back(byte);
            }
            size = num_bits;
            return data;
        }
        std::string decompress(std::vector<uint8_t> const &data)
        {
            return decompress_tree_.decompress(data);
        }
        std::string decompress(std::vector<char> const &data)
        {
            std::vector<uint8_t> byte_data(data.size());
            std::transform(data.cbegin(), data.cend(), byte_data.begin(), [](char b) -> uint8_t
                           { return static_cast<uint8_t>(b); });
            return decompress_tree_.decompress(byte_data);
        }
    };

} // namespace shannon_fano

#endif
