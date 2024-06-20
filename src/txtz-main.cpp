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
#include <unordered_map>
#include <vector>

#include "getopt.hpp"
#include "mappings.hpp"
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
}

auto is_deleter = [](std::istream *ptr) -> void
{
    if (ptr != nullptr && ptr != &std::cin)
    {
        static_cast<std::ifstream *>(ptr)->close();
        delete ptr;
    }
};

auto os_deleter = [](std::ostream *ptr) -> void
{
    if (ptr != nullptr && ptr != &std::cout)
    {
        static_cast<std::ofstream *>(ptr)->close();
        delete ptr;
    }
};

int main(int argc, char *argv[])
{
    using argparser = argparser::argparser;
    argparser opt(argc, argv);
    opmode_t op = INVALID_OP;
    bool strip_crlf = true;
    bool stats_only_output = false;
    std::string input_filename;
    std::string output_filename;
    std::unique_ptr<std::istream, decltype(is_deleter)> in{nullptr, is_deleter};
    std::unique_ptr<std::ostream, decltype(os_deleter)> out{nullptr, os_deleter};
    opt
        .info("txtz", argv[0])
        .help({"-?", "--help"}, "Display this help")
        .reg({"-d", "--decompress"}, argparser::no_argument, "Decompress data", [&op](std::string const &)
             { op = DECOMPRESS; })
        .reg({"-c", "--compress"}, argparser::no_argument, "Compress data", [&op](std::string const &)
             { op = COMPRESS; })
        .reg({"--no-remove-crlf"}, argparser::no_argument, "Don't remove CR/LF from input.", [&strip_crlf](std::string const &)
             { strip_crlf = false; })
        .reg({"--stats", "--stats-only"}, argparser::no_argument, "Only output compression statistics.", [&stats_only_output](std::string const &)
             { stats_only_output = true; })
        .reg({"-i", "--input-file"}, "INPUT_FILENAME", argparser::required_argument, "input file", [&input_filename](std::string const &arg)
             { input_filename = arg; })
        .reg({"-o", "--output-file"}, "OUTPUT_FILENAME", argparser::required_argument, "Where the output goes to", [&output_filename](std::string const &arg)
             { output_filename = arg; });
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

    if (op == INVALID_OP)
    {
        std::cerr << "\u001b[31;1mERROR: operation mode missing, please give -c or -d.\u001b[0m\n";
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

    if (output_filename.empty())
    {
        out.reset(&std::cout);
    }
    else
    {
        out.reset(new std::ofstream(output_filename, std::ios::trunc | std::ios::binary));
    }

    std::vector<char> in_buf(std::istreambuf_iterator<char>(*in), {});
    txtz::txtz z(txtz::compression_table);

    switch (op)
    {
    case COMPRESS:
    {
        if (!stats_only_output)
        {
            std::cout << "Compressing with " <<
#if defined(ALGO_HUFFMAN)
                "Huffmann"
#elif defined(ALGO_SHANNON_FANO)
                "Shannon-Fano"
#else
#error "Invalid map building algorithm. Define one of ALGO_HUFFMAN or ALGO_SHANNON_FANO!"
#endif
                      << " encoded binary tree ...\n";
        }
        std::string s;
        if (strip_crlf)
        {
            std::copy_if(std::begin(in_buf), std::end(in_buf), std::back_inserter(s), [](char c)
                         { return c != '\r' && c != '\n'; });
        }
        std::vector<uint8_t> out_buf;
        std::size_t sz;
        out_buf = z.compress(s, sz);
        if (stats_only_output)
        {
            std::cout << std::setprecision(3) << 100 * float(out_buf.size()) / (float(s.size())) << '\n';
        }
        else
        {
            std::copy(std::begin(out_buf), std::end(out_buf), std::ostream_iterator<char>(*out));
            std::cout << (8 * s.size()) << " bits -> " << sz << " bits (" << out_buf.size() << " bytes), compressed to " << std::setprecision(3) << 100 * float(out_buf.size()) / (float(s.size())) << "% of original size.\n";
        }
        break;
    }
    case DECOMPRESS:
    {
        if (!stats_only_output)
            std::cout << "Decompressing ...\n";
        auto out_buf = z.decompress(in_buf);
        std::copy(std::begin(out_buf), std::end(out_buf), std::ostream_iterator<char>(*out));
        std::cout << '\n'
                  << (8 * in_buf.size()) << " -> " << (8 * out_buf.size()) << '\n';
        break;
    }
    case INVALID_OP: // will never be reached
        break;
    }

    return EXIT_SUCCESS;
}
