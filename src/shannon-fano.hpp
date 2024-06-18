/*

 Copyright (c) 2023-2024 Oliver Lau <oliver.lau@gmail.com>

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

#ifndef __SHANNON_FANO_HPP__
#define __SHANNON_FANO_HPP__

#include <bitset>
#include <cstdlib>
#include <iostream>
#include <string>
#include <sstream>
#include <unordered_map>
#include <vector>

#include "bintree.hpp"

namespace shannon_fano
{
    typedef uint32_t code_t;

    class code
    {
        uint32_t bitcount_;
        code_t bits_;

    public:
        code()
            : bitcount_(0), bits_(0) {}
        code(uint32_t len, code_t bits)
            : bitcount_(len), bits_(bits) {}
        code(code const &o)
            : bitcount_(o.bitcount_), bits_(o.bits_) {}

        inline void operator=(code const &o)
        {
            bitcount_ = o.bitcount_;
            bits_ = o.bits_;
        }

        inline uint32_t bitcount(void) const
        {
            return bitcount_;
        }

        inline code_t bits(void) const
        {
            return bits_;
        }

        inline void append(bool bit)
        {
            bits_ |= code_t(bit) << bitcount_++;
        }

        inline code operator+(bool bit)
        {
            code x{*this};
            x.append(bit);
            return x;
        }

        std::string str(void) const
        {
            std::ostringstream oss;
            oss << *this;
            return oss.str();
        }

        friend std::ostream &operator<<(std::ostream &os, code const &c)
        {
            for (code_t mask = 1U << (c.bitcount_ - 1); mask != 0; mask >>= 1)
            {
                os << (((c.bits_ & mask) == 0) ? '0' : '1');
            }
            return os;
        }
    };


    /**
     * A struct representing an n-gram in a binary tree.
     */
    struct ngram_t
    {
        /**
         * The actual string of the n-gram.
         */
        std::string token{};
        /**
         * The larger the value the more frequent the n-gram is.
         */
        float weight{0};
        /**
         * Depending on the `weight` of the n-gram a path through 
         * a binary tree down to the n-gram is calculated.
         */
        code c;
    };

    /**
     * Build a binary tree with all of the n-grams given. 
     * 
     * Update `shannon_fano::code` field of each n-gram to reflect
     * the path to the corresponding node in the binary tree.
     * 
     * @param p sorted list of n-grams
     */
    void update(std::vector<ngram_t> &p);


    /**
     * A class to efficiently compress and decompress short strings.
     */
    class txtz
    {
        /**
         * For compression a lookup table is needed. Each entry is
         * indexed by the word to look up. The entry of type `code`
         * contains a prefix free bit sequence along with a value
         * for its length.
         */
        std::unordered_map<std::string, code> compress_table_;

        /**
         * For decompression a binary tree is needed.
         */
        bintree<code_t, uint32_t, std::string, uint8_t> decompress_tree_;

        std::size_t max_token_length_{};

    public:
        explicit txtz(std::unordered_map<std::string, code> const &table)
            : compress_table_(table)
        {
            for (auto const &it : compress_table_)
            {
                decompress_tree_.append(it.second.bits(), it.second.bitcount(), it.first);
                if (it.first.size() > max_token_length_)
                {
                    max_token_length_ = it.first.size();
                }
            }
        }

        std::vector<uint8_t> compress(std::string const &str, std::size_t &size)
        {
            std::vector<uint8_t> data;
            std::size_t max_len = std::min(max_token_length_, str.size());
            // write one byte with length of string
            data.push_back(static_cast<uint8_t>(str.size()));
            auto it = std::begin(str);
            auto chunk_end = std::min(std::begin(str) + max_len, std::end(str));
            auto last = std::end(str);
            uint8_t byte = 0;
            std::size_t num_bits = 0;
            int bit_idx = 0;
            while (it < last)
            {
                while (it < chunk_end)
                {
                    std::string token(it, chunk_end);
                    std::cout << " ? " << token << "\n";
                    if (compress_table_.find(token) != compress_table_.end())
                    {
                        code const &c = compress_table_.at(token);
                        code_t value = c.bits() << (8 * sizeof(code_t) - c.bitcount());
                        num_bits += c.bitcount();
                        // XXX: slow bit by bit encoder
                        for (std::size_t i = 0; i < c.bitcount(); ++i)
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
                        std::cout << "Found " << token << ", encoded to " << c << "\n";
                        it += token.size();
                        chunk_end = std::min(it + max_len, std::end(str));
                    }
                    else
                    {
                        --chunk_end;
                    }
                }
            }
            auto remaining_bits = data.size() * 8 - num_bits;
            if (remaining_bits > 0)
            {
                byte <<= remaining_bits;
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
            std::transform(std::begin(data), std::end(data), std::begin(byte_data), [](char b) -> uint8_t
                           { return static_cast<uint8_t>(b); });
            return decompress_tree_.decompress(byte_data);
        }
    };

} // namespace shannon_fano

#endif
