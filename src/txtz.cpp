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

#include "shannon-fano.hpp"

namespace shannon_fano
{

    txtz::txtz(std::unordered_map<std::string, code> const &table)
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

    std::vector<uint8_t> txtz::compress(std::string const &str, std::size_t &size)
    {
        std::vector<uint8_t> compressed_data;
        const std::string &uncompressed = str + STOP_TOKEN;
        const std::size_t max_len = std::min(max_token_length_, uncompressed.size());
        auto it = std::begin(uncompressed);
        auto chunk_end = std::min(std::begin(uncompressed) + max_len, std::end(uncompressed));
        const auto last = std::end(uncompressed);
        uint8_t byte = 0;
        long num_bits = 0;
        int bit_idx = 0;
        while (it < last)
        {
            while (it < chunk_end)
            {
                std::string token(it, chunk_end);
                if (compress_table_.find(token) != compress_table_.end())
                {
                    code const &c = compress_table_.at(token);
                    code_t value = c.bits() << (8 * sizeof(code_t) - c.bitcount());
                    num_bits += static_cast<long>(c.bitcount());
                    // XXX: slow bit by bit encoder
                    for (std::size_t i = 0; i < c.bitcount(); ++i)
                    {
                        byte <<= 1;
                        constexpr code_t mask = (1ULL << (8 * sizeof(code_t) - 1));
                        if ((value & mask) != 0)
                        {
                            byte |= 1;
                        }
                        else
                        {
                        }
                        value <<= 1;
                        if (++bit_idx == 8)
                        {
                            compressed_data.push_back(byte);
                            byte = 0b00000000;
                            bit_idx = 0;
                        }
                    }
                    it += token.size();
                    chunk_end = std::min(it + max_len, std::end(uncompressed));
                }
                else
                {
                    --chunk_end;
                }
            }
        }
        if (bit_idx > 0)
        {
            byte <<= 8 - bit_idx;
            compressed_data.push_back(byte);
        }
        size = num_bits;
        return compressed_data;
    }

    std::string txtz::decompress(std::vector<uint8_t> const &data)
    {
        return decompress_tree_.decompress(data);
    }

    std::string txtz::decompress(std::vector<char> const &data)
    {
        std::vector<uint8_t> byte_data(data.size());
        std::transform(std::begin(data), std::end(data), std::begin(byte_data), [](char b) -> uint8_t
                       { return static_cast<uint8_t>(b); });
        return decompress_tree_.decompress(byte_data);
    }

}
