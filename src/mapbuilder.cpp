/*
    Copyright (c) 2023 Oliver Lau
*/

#include <algorithm>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
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

#include "shannon-fano.hpp"

using json = nlohmann::json;
namespace fs = std::filesystem;
namespace loc = boost::locale;
namespace sf = shannon_fano;

constexpr std::size_t max_codepoints_per_grapheme = 4;
constexpr std::size_t max_gram = 4;

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        std::cerr << "\u001b[31;1mERROR: input filename missing.\u001b[0m\n";
        return EXIT_FAILURE;
    }
    fs::path path = argv[1];
    std::unordered_map<std::string, std::size_t> monograms;
    std::array<char, 4> progress_indicator{'-', '\\', '|', '/'};
    int pi_idx = 0;
    auto filenames = glob::glob(argv[1]);
    while (filenames)
    {
        std::string const &filename = filenames.current_match();
        filenames.next();
        std::cout << "\rProcessing " << (path.parent_path() / filename).string() << " ... \u001b[K" << std::flush;
        std::ifstream in(path.parent_path() / filename, std::ios::binary);
        json data;
        try
        {
            data = json::parse(in);
        }
        catch (const std::exception &e)
        {
            std::cerr << e.what() << '\n';
            continue;
        }
        if (data.empty())
            continue;

        for (auto const &a : data)
        {
            if (!a.contains("text_clean") || !a.at("text_clean").is_string())
                continue;
            std::cout << progress_indicator.at((pi_idx++) % progress_indicator.size()) << "\u001b[D" << std::flush;
            std::string u8_text = a.at("text_clean").get<std::string>();
            u8_text = std::regex_replace(u8_text, std::regex("(\n\r|\r\n|\n)+"), " ");
            if (u8_text.size() < max_codepoints_per_grapheme * max_gram)
                continue;
            loc::generator gen;
            loc::boundary::ssegment_index map(loc::boundary::character, u8_text.begin(), u8_text.end(), gen("de_DE.UTF-8"));
            loc::boundary::ssegment_index::iterator e = map.end();
            for (loc::boundary::ssegment_index::iterator it = map.begin(); it != e; ++it)
            {
                monograms[it->str()] += 1;
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

    if (monograms.empty())
        return EXIT_SUCCESS;

    std::cout
        << '\n'
        << std::setw(6) << monograms.size() << " monograms; max. `" << max_monogram->first << "`   : " << max_monogram->second << "\n"
        << '\n';

    for (int i = 0; i < 256; ++i)
    {
        char const c = static_cast<char>(i);
        std::string const token(&c, 1);
        if (monograms.find(token) == monograms.end())
        {
            monograms[token] = 1;
        }
    }

    std::vector<sf::ngram_t> ngrams;
    std::transform(std::begin(monograms), std::end(monograms), std::back_inserter(ngrams), [](decltype(monograms)::value_type it) -> sf::ngram_t
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
        std::cout << ngram.token << ": " << ngram.weight << '\n';
        result["decompress"][ngram.c.str()] = util::escaped(ngram.token);
        result["compress"][util::escaped(ngram.token)] = json::object_t{
            {"l", ngram.c.length()},
            {"v", ngram.c.bits()}};
        cpp << "    /* " << ngram.token << " */ {\"" << util::escaped(ngram.token) << "\", code(" << std::dec << ngram.c.length() << ", 0b" << ngram.c.str() << ")},\n";
    }
    cpp << "  };\n"
        << "}\n";

    for (auto const &ngram : ngrams)
    {
        std::cout << ngram.token << ": " << ngram.weight << ' ' << ngram.c << '\n';
    }

    std::ofstream out("de_DE-utf8.json", std::ios::binary | std::ios::trunc);
    out << result.dump();

    std::ofstream cppout("../src/de_DE-utf8.cpp", std::ios::binary | std::ios::trunc);
    cppout << cpp.str();

    return EXIT_SUCCESS;
}
