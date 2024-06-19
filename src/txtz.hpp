#ifndef __TXTZ_HPP__
#define __TXTZ_HPP__

#include <cstddef>
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
        static constexpr char STOP_TOKEN = '\xff';
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
        bintree<code_t, uint32_t, std::string, uint8_t> decompress_tree_{std::string(&STOP_TOKEN, 1)};
        std::size_t max_token_length_{};
    };
}

#endif