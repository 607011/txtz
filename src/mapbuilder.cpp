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
#include <iomanip>
#include <iostream>
#include <limits>
#include <regex>
#include <sstream>
#include <stdexcept>
#include <string_view>
#include <unordered_map>
#include <utility>
#include <vector>

#include <nlohmann/json.hpp>
#include <glob.h>
#include <getopt.hpp>

#include "shannon-fano.hpp"

using json = nlohmann::json;
namespace fs = std::filesystem;
namespace sf = shannon_fano;

#ifndef MAP_FILE
#define MAP_FILE "mappings"
#endif

int main(int argc, char *argv[])
{
    using argparser = argparser::argparser;
    std::string table_name = MAP_FILE;
    bool split_by_phomenes = false;
    char phoneme_delim = '|';
    bool fill_missing_monograms = true;
    char histo_delim = ';';
    bool with_histogram = true;
    bool generate_json = false;
    std::vector<fs::path> input_paths;
    int verbosity{};
    argparser opt(argc, argv);

    opt
        .info("txtz token map builder", argv[0])
        .help({"-?", "--help"}, "Display this help")
        .reg({"-v"}, argparser::no_argument,
             "Increase verbosity of output",
             [&verbosity](std::string const &)
             {
                 ++verbosity;
             })
        .reg({"-i", "--input", "--input-file"}, "INPUT", argparser::required_argument,
             "Use option multiple times to add as many input file as you want.",
             [&input_paths](std::string const &arg)
             {
                 input_paths.push_back(fs::path(arg));
             })
        .reg({"-m", "--map-file"}, "MAP_FILENAME", argparser::required_argument,
             "Name of C++ and JSON file to generate (default: \"" + table_name + "\").",
             [&table_name](std::string const &arg)
             {
                 table_name = arg;
             })
        .reg({"-p", "--phoneme-delimiter", "--phoneme-delim"}, argparser::required_argument,
             std::string("Phoneme delimiter (default: \"") + phoneme_delim + "\").",
             [&split_by_phomenes, &phoneme_delim](std::string const &arg)
             {
                 split_by_phomenes = true;
                 phoneme_delim = arg.front();
             })
        .reg({"--no-fill-missing-monograms"}, argparser::no_argument,
             "Don't add missing monograms.",
             [&fill_missing_monograms](std::string const &)
             {
                 fill_missing_monograms = false;
             })
        .reg({"--no-histo", "--no-histogram"}, argparser::no_argument,
             "The input files do not contain frequency data.",
             [&with_histogram](std::string const &)
             {
                 with_histogram = false;
             })
        .reg({"--json"}, argparser::no_argument,
             "Generate JSON map file in addition to C++ map file.",
             [&generate_json](std::string const &)
             {
                 generate_json = true;
             })
        .reg({"--histo-delim"}, argparser::required_argument,
             std::string("Histogram data delimiter (default: \"") + histo_delim + "\").",
             [&histo_delim](std::string const &arg)
             {
                 histo_delim = arg.front();
             });
    try
    {
        opt();
    }
    catch (::argparser::argument_required_exception const &e)
    {
        std::cerr << "ERROR: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    catch (::argparser::unknown_option_exception const &e)
    {
        std::cerr << "ERROR: " << e.what() << std::endl;
        opt.display_help();
        return EXIT_FAILURE;
    }
    if (input_paths.empty())
    {
        std::cerr << "\u001b[31;1mERROR: input missing (see option -i)\n";
        return EXIT_FAILURE;
    }

    std::unordered_map<std::string, float> tokens;
    for (auto const &input_path : input_paths)
    {
        auto filenames = glob::glob(input_path.string());
        while (filenames)
        {
            std::string const &filename = filenames.current_match();
            filenames.next();
            if (!fs::exists(input_path.parent_path() / filename))
            {
                std::cerr << "\u001b[31;1mERROR: file `"
                          << (input_path.parent_path() / filename).string()
                          << "` does not exist\n";
                continue;
            }
            if (verbosity > 0)
            {
                std::cout
                    << "\rProcessing " << (input_path.parent_path() / filename).string() << " ... \u001b[K\n"
                    << std::flush;
            }
            std::ifstream in(input_path.parent_path() / filename, std::ios::binary);
            std::string line;
            while (std::getline(in, line))
            {
                line = std::regex_replace(line, std::regex("(\n\r|\r\n|\n)+"), " ");
                if (with_histogram)
                {
                    float weight = std::numeric_limits<float>::epsilon();
                    auto word_histo = util::unpair(line, histo_delim);
                    try
                    {
                        weight = std::stof(word_histo.second);
                    }
                    catch (std::invalid_argument const &e)
                    {
                        std::cerr << e.what() << " in line " << line << "\"\n";
                    }
                    catch (std::out_of_range const &e)
                    {
                        std::cerr << e.what() << " in line " << line << "\"\n";
                    }
                    if (split_by_phomenes)
                    {
                        for (auto const &ph : util::split(word_histo.first, phoneme_delim))
                        {
                            tokens[ph] += weight /* * ph.size() */;
                        }
                    }
                    else
                    {
                        word_histo.first.erase(std::remove_if(std::begin(word_histo.first), std::end(word_histo.first), [&phoneme_delim](char c) {
                            return c == phoneme_delim;
                        }), std::end(word_histo.first));
                        tokens[word_histo.first] += weight /* * word_histo.first.size() */;
                    }
                }
            }
        }
    }

    using pair_type = decltype(tokens)::value_type;
    auto max_tokens = std::max_element(
        std::begin(tokens), std::end(tokens),
        [](const pair_type &p1, const pair_type &p2)
        {
            return p1.second < p2.second;
        });

    if (verbosity > 0)
    {
        std::cout << "\nStatistics:\n";
        if (!tokens.empty())
        {
            std::cout << " - " << std::setw(6) << tokens.size() << " phonemes; max. `" << max_tokens->first << "`   : " << max_tokens->second << '\n';
        }
        std::cout << std::endl;
    }

    if (fill_missing_monograms)
    {
        for (uint8_t t = 0U; t != 255U; ++t)
        {
            char c = static_cast<char>(t);
            std::string token(&c, 1);
            if (tokens.find(token) == std::end(tokens))
            {
                tokens[token] = 1;
            }
        }
    }

    // tokens["\0"] = 1e7f;

    std::vector<sf::ngram_t> ngrams;
    std::transform(std::begin(tokens), std::end(tokens), std::back_inserter(ngrams), [](decltype(tokens)::value_type it) -> sf::ngram_t
                   { return sf::ngram_t{it.first, it.second}; });
    std::sort(std::begin(ngrams), std::end(ngrams), [](decltype(ngrams)::value_type a, decltype(ngrams)::value_type b)
              { return a.weight > b.weight; });


    for (int i = 0; i < 200; ++i)
    {
        std::cout << ngrams.at(i).token << ": " << ngrams.at(i).weight << "\n";
    }

    sf::update(ngrams);

    if (verbosity > 0)
    {
        std::cout << "Writing ..." << std::flush;
    }
    std::ostringstream cpp;
    cpp << "#include <string>\n"
        << "#include <unordered_map>\n"
        << "#include \"shannon-fano.hpp\"\n"
        << "namespace shannon_fano\n"
        << "{\n"
        << "  std::unordered_map<std::string, code> compression_table = {\n";

    for (auto const &ngram : ngrams)
    {
        cpp << "   {\"" << util::escaped(ngram.token) << "\", code(" << std::dec << ngram.c.bitcount() << ", 0b" << ngram.c.str() << ")},\n";
    }
    cpp << "  };\n"
        << "}\n";
    std::ofstream cppout(table_name + ".cpp", std::ios::binary | std::ios::trunc);
    cppout << cpp.str();

    if (generate_json)
    {
        json result;
        for (auto const &ngram : ngrams)
        {
            result["decompress"][ngram.c.str()] = util::escaped(ngram.token);
            result["compress"][util::escaped(ngram.token)] = json::object_t{
                {"l", ngram.c.bitcount()},
                {"v", ngram.c.bits()}};
        }
        std::ofstream out(table_name + ".json", std::ios::binary | std::ios::trunc);
        out << result.dump(2);
    }

    if (verbosity > 1)
    {
        for (auto const &ngram : ngrams)
        {
            std::cout << ngram.token << ": " << ngram.weight << ' ' << ngram.c << '\n';
        }
    }


    return EXIT_SUCCESS;
}
