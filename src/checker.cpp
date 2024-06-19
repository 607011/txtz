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

#include <algorithm>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <memory>
#include <regex>
#include <sstream>
#include <vector>

#include "getopt.hpp"
#include "mappings.hpp"
#include "code.hpp"
#include "txtz.hpp"
#include "shannon-fano.hpp"

namespace fs = std::filesystem;

namespace
{
    typedef enum
    {
        INVALID_OP,
        COMPRESS,
        DECOMPRESS,
    } opmode_t;

    typedef enum
    {
        INVALID_ALGO,
        SMAZ,
        SHANNON_FANO,
    } algo_t;
}

auto is_deleter = [](std::istream *ptr) -> void
{
    if (ptr != nullptr && ptr != &std::cin)
    {
        static_cast<std::ifstream *>(ptr)->close();
        delete ptr;
    }
};

int main(int argc, char *argv[])
{
    using argparser = argparser::argparser;
    argparser opt(argc, argv);
    char histo_delim = ';';
    char phoneme_delim = '|';
    std::string input_filename;
    std::unique_ptr<std::istream, decltype(is_deleter)> in{nullptr, is_deleter};
    opt
        .info("txtz", argv[0])
        .help({"-?", "--help"}, "Display this help")
        .pos("INPUT_FILENAME", "input file", [&input_filename](std::string const &arg)
             { input_filename = arg; });
    try
    {
        opt();
    }
    catch (::argparser::help_requested_exception const &)
    {
        return EXIT_SUCCESS;
    }
    catch (::argparser::argument_required_exception const &e)
    {
        std::cerr << "\u001b[31;1mERROR: " << e.what() << "\u001b[0m\n";
        return EXIT_FAILURE;
    }
    catch (std::exception const &e)
    {
        std::cerr << "\u001b[31;1mERROR: " << e.what() << "\u001b[0m\n";
        return EXIT_FAILURE;
    }

    if (input_filename.empty())
    {
        in.reset(&std::cin);
    }
    else
    {
        if (!fs::exists(input_filename))
        {
            std::cerr << "\u001b[31;1mERROR: input file '" << input_filename << "' doesn't exist.\u001b[0m\n";
            return EXIT_FAILURE;
        }
        in.reset(new std::ifstream(input_filename, std::ios::binary));
    }

    float sum_compression_rates = 0;
    std::size_t rate_count = 0;
    txtz::txtz z(txtz::compression_table);
    std::string line;
    while (std::getline(*in, line))
    {
        auto word_histo = util::unpair(line, histo_delim);
        // remove phoneme delimiters
        word_histo.first.erase(std::remove_if(std::begin(word_histo.first), std::end(word_histo.first), [&phoneme_delim](char c)
                                              { return c == phoneme_delim; }),
                               std::end(word_histo.first));
        const auto weight = std::stoull(word_histo.second);
        // remove all CR/LF
        std::string s;
        std::copy_if(std::begin(word_histo.first), std::end(word_histo.first), std::back_inserter(s), [](char c)
                     { return c != '\r' && c != '\n'; });
        std::cout << s << " (" << weight << ") ";
        std::size_t sz;
        std::vector<uint8_t> out_buf;
        out_buf = z.compress(s, sz);
        sum_compression_rates += float(out_buf.size()) / (float(s.size())) * weight;
        rate_count += weight;
        const std::string &out_word = z.decompress(out_buf);
        if (out_word == word_histo.first)
        {
            std::cout << "\t\u001b[32;1mOK\u001b[0m "
                      << std::setprecision(3) << 100 * float(out_buf.size()) / (float(s.size()))
                      << "%\n";
        }
        else
        {
            std::cout << "\u001b[31;1mERROR\u001b[0m\n";
            return EXIT_FAILURE;
        }
    }
    std::cout << "\nSUCCESS!\n";
    std::cout << "avg. compression rate: " << 1e2f * sum_compression_rates / rate_count << "%\n";
    return EXIT_SUCCESS;
}
