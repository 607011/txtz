/*
    Copyright (c) 2023 Oliver Lau
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

#include <boost/locale.hpp>

#include <nlohmann/json.hpp>
#include <glob.h>
#include <getopt.hpp>

#include "shannon-fano.hpp"

using json = nlohmann::json;
namespace fs = std::filesystem;
namespace loc = boost::locale;
namespace sf = shannon_fano;

int main(int argc, char *argv[])
{
    using argparser = argparser::argparser;
    loc::boundary::boundary_type boundary = loc::boundary::boundary_type::character;
    std::string generator_string = "de_DE.UTF-8";
    bool split_by_phomenes = false;
    char phoneme_delim = '|';
    bool fill_missing_monograms = false;
    char histo_delim = '\t';
    bool with_histogram = false;
    fs::path input_path;
    argparser opt(argc, argv);
    opt
        .reg({"-i", "--input"}, argparser::required_argument,
             [&input_path](std::string const &arg)
             {
                 input_path = arg;
             })
        .reg({"-g", "--generator"}, argparser::required_argument,
             [&generator_string](std::string const &arg)
             {
                 generator_string = arg;
             })
        .reg({"-p", "--phonemes"}, argparser::required_argument,
             [&split_by_phomenes, &phoneme_delim, &boundary](std::string const &arg)
             {
                 split_by_phomenes = true;
                 phoneme_delim = arg.front();
                 boundary = loc::boundary::boundary_type::word;
             })
        .reg({"--fill-missing-monograms"}, argparser::no_argument,
             [&fill_missing_monograms](std::string const &)
             {
                 fill_missing_monograms = true;
             })
        .reg({"-h", "--with-histo", "--with-histogram"}, argparser::no_argument,
             [&with_histogram](std::string const &)
             {
                 with_histogram = true;
             })
        .reg({"--histo-delim"}, argparser::required_argument,
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
        std::cerr << "ERROR in command line arguments\n";
        return EXIT_FAILURE;
    }
    if (input_path.empty())
    {
        std::cerr << "\u001b[31;1mERROR: input path missing (see option -i)\n";
        return EXIT_FAILURE;
    }

    std::unordered_map<std::string, float> monograms;
    std::unordered_map<std::string, float> phonemes;
    // std::array<char, 4> const progress_indicator{'-', '\\', '|', '/'};
    // int pi_idx = 0;
    // std::size_t line_no = 0;
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
        std::cout
            << "\rProcessing " << (input_path.parent_path() / filename).string() << " ... \u001b[K\n"
            << std::flush;
        std::ifstream in(input_path.parent_path() / filename, std::ios::binary);
        std::string line;
        loc::generator gen;
        while (std::getline(in, line))
        {
            // std::cout << progress_indicator.at((pi_idx++) % progress_indicator.size()) << "\u001b[D" << std::flush;
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
                    std::cerr << e.what() << '\n';
                }
                catch (std::out_of_range const &e)
                {
                    std::cerr << e.what() << '\n';
                }
                if (split_by_phomenes)
                {
                    for (auto const &ph : util::split(word_histo.first, phoneme_delim))
                    {
                        std::cout << "`" << ph << "`: " << weight << '\n';
                        phonemes[ph] += weight;
                    }
                }
            }
            else
            {
                loc::boundary::ssegment_index map(boundary, line.begin(), line.end(), gen(generator_string));
                loc::boundary::ssegment_index::iterator e = map.end();
                for (loc::boundary::ssegment_index::iterator it = map.begin(); it != e; ++it)
                {
                    monograms[it->str()] += 1;
                }
            }
        }
    }

    using pair_type = decltype(monograms)::value_type;
    auto max_monogram = std::max_element(
        std::begin(monograms), std::end(monograms),
        [](const pair_type &p1, const pair_type &p2)
        {
            return p1.second < p2.second;
        });
    auto max_phonemes = std::max_element(
        std::begin(phonemes), std::end(phonemes),
        [](const pair_type &p1, const pair_type &p2)
        {
            return p1.second < p2.second;
        });

    std::cout << '\n';
    if (!monograms.empty())
    {
        std::cout << std::setw(6) << monograms.size() << " monograms; max. `" << max_monogram->first << "`   : " << max_monogram->second << '\n';
    }
    if (!phonemes.empty())
    {
        std::cout << std::setw(6) << phonemes.size() << " phonemes; max. `" << max_phonemes->first << "`   : " << max_phonemes->second << '\n';
    }
    std::cout << '\n';

    if (fill_missing_monograms)
    {
        for (unsigned char i = 0; i != 255; ++i)
        {
            char c = static_cast<char>(i);
            std::string token(&c, 1);
            if (monograms.find(token) == monograms.end())
            {
                monograms[token] = 1;
            }
        }
    }

    std::vector<sf::ngram_t> ngrams;
    std::transform(std::begin(monograms), std::end(monograms), std::back_inserter(ngrams), [](decltype(monograms)::value_type it) -> sf::ngram_t
                   { return sf::ngram_t{it.first, it.second}; });
    std::transform(std::begin(phonemes), std::end(phonemes), std::back_inserter(ngrams), [](decltype(phonemes)::value_type it) -> sf::ngram_t
                   { return sf::ngram_t{it.first, it.second}; });
    std::sort(std::begin(ngrams), std::end(ngrams), [](decltype(ngrams)::value_type a, decltype(ngrams)::value_type b)
              { return a.weight > b.weight; });
    sf::update(ngrams);

    std::ostringstream cpp;
    cpp << "#include <string>\n"
        << "#include <unordered_map>\n"
        << "#include \"shannon-fano.hpp\"\n"
        << "namespace shannon_fano\n"
        << "{\n"
        << "  std::unordered_map<std::string, code> compression_table = {\n";
    json result;
    for (auto const &ngram : ngrams)
    {
        result["decompress"][ngram.c.str()] = util::escaped(ngram.token);
        result["compress"][util::escaped(ngram.token)] = json::object_t{
            {"l", ngram.c.length()},
            {"v", ngram.c.bits()}};
        cpp << "    /* " << ngram.token << " */ {\"" << util::escaped(ngram.token) << "\", code(" << std::dec << ngram.c.length() << ", 0b" << ngram.c.str() << ")},\n";
    }
    cpp << "  };\n"
        << "}\n";

    // for (auto const &ngram : ngrams)
    // {
    //     std::cout << ngram.token << ": " << ngram.weight << ' ' << ngram.c << '\n';
    // }

    std::ofstream out(generator_string + ".json", std::ios::binary | std::ios::trunc);
    out << result.dump();

    std::ofstream cppout(generator_string + ".cpp", std::ios::binary | std::ios::trunc);
    cppout << cpp.str();

    return EXIT_SUCCESS;
}
