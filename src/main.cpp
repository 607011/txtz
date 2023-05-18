/*
    Copyright (c) 2023 Oliver Lau
*/

#include <algorithm>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <memory>
#include <vector>

extern "C"
{
#include "smaz.h"
}
#include "getopt.hpp"

namespace fs = std::filesystem;

namespace
{
    typedef enum
    {
        INVALID,
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
    opmode_t op = INVALID;
    std::string input_filename;
    std::string output_filename;
    std::unique_ptr<std::istream, decltype(is_deleter)> in{nullptr, is_deleter};
    std::unique_ptr<std::ostream, decltype(os_deleter)> out{nullptr, os_deleter};
    opt
        .reg({"-d", "--decompress"}, argparser::no_argument, [&op](std::string const &)
             { op = DECOMPRESS; })
        .reg({"-c", "--compress"}, argparser::no_argument, [&op](std::string const &)
             { op = COMPRESS; })
        .reg({"-o", "--output"}, argparser::required_argument, [&output_filename](std::string const &arg)
             { output_filename = arg; })
        .reg({"-i", "--input"}, argparser::required_argument, [&input_filename](std::string const &arg)
             { input_filename = arg; });
    try
    {
        opt();
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

    if (op == INVALID)
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
        in.reset(new std::ifstream(input_filename));
    }

    if (output_filename.empty())
    {
        out.reset(&std::cout);
    }
    else
    {
        out.reset(new std::ofstream(output_filename));
    }

    auto in_flags = in->flags();
    auto out_flags = out->flags();
    in->setf(std::ios::binary);
    out->setf(std::ios::binary);

    std::size_t out_len;
    std::vector<char> in_buf(std::istreambuf_iterator<char>(*in), {});
    constexpr std::size_t SAFETY_MARGIN = 10;
    std::vector<char> out_buf(SAFETY_MARGIN * in_buf.size());
    switch (op)
    {
    case COMPRESS:
    {
        out_len = static_cast<std::size_t>(smaz_compress(in_buf.data(), static_cast<int>(in_buf.size()), out_buf.data(), static_cast<int>(out_buf.size())));
        out_buf.resize(static_cast<std::size_t>(out_len));
        break;
    }
    case DECOMPRESS:
    {
        out_len = static_cast<std::size_t>(smaz_decompress(in_buf.data(), static_cast<int>(in_buf.size()), out_buf.data(), static_cast<int>(out_buf.size())));
        out_buf.resize(static_cast<std::size_t>(out_len));
        break;
    }
    case INVALID: // will never be reached
        break;
    }
    std::copy(std::begin(out_buf), std::end(out_buf), std::ostream_iterator<char>(*out));

    in->setf(in_flags);
    out->setf(out_flags);
    return EXIT_SUCCESS;
}
