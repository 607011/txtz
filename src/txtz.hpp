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

#ifndef __TXTZ_HPP__
#define __TXTZ_HPP__

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

#include "bintree.hpp"
#include "code.hpp"

namespace txtz
{

    /**
     * A class to efficiently compress and decompress short strings.
     */
    class txtz final
    {
    public:
        static constexpr char STOP_TOKEN = '\xff'; // 0xFF isn't found in any UTF-8 continuation bytes
        explicit txtz(std::unordered_map<std::string, code> const &);
        std::vector<uint8_t> compress(std::string const &, std::size_t &);
        std::string decompress(std::vector<uint8_t> const &);
        std::string decompress(std::vector<char> const &);

    private:
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
        bintree decompress_tree_{std::string(&STOP_TOKEN, 1)};
        std::size_t max_token_length_{};
    };
}

#endif
