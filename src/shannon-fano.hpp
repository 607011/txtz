/*

 Copyright (c) 2023-2024 Oliver Lau <oliver@ersatzworld.net>

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
    typedef unsigned long code_t;

    class code
    {
        unsigned long bitcount_;
        code_t bits_;

    public:
        code();
        code(unsigned long len, code_t bits);
        code(code const &o);

        void operator=(code const &o);
        unsigned long bitcount(void) const;
        code_t bits(void) const;
        void append(bool bit);
        code operator+(bool bit);
        std::string str(void) const;

        friend std::ostream &operator<<(std::ostream &os, code const &c)
        {
            auto b = c.bits_;
            for (unsigned long p = 0; p != c.bitcount_; ++p)
            {
                os << (b & 1);
                b >>= 1;
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
        explicit txtz(std::unordered_map<std::string, code> const &table);
        std::vector<uint8_t> compress(std::string const &str, std::size_t &size);
        std::string decompress(std::vector<uint8_t> const &data);
        std::string decompress(std::vector<char> const &data);
    };

} // namespace shannon_fano

#endif
