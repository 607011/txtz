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

#include "nlohmann/json.hpp"
#include "glob.h"

using json = nlohmann::json;
namespace fs = std::filesystem;
namespace loc = boost::locale;

struct ngram_t
{
    std::string token{};
    std::size_t count{};
};

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        std::cerr << "\u001b[31;1mERROR: input filename missing.\u001b[0m\n";
        return EXIT_FAILURE;
    }
    fs::path path = argv[1];

    std::unordered_map<std::string, std::size_t> monograms;
    std::unordered_map<std::string, std::size_t> digrams;
    std::unordered_map<std::string, std::size_t> trigrams;
    std::unordered_map<std::string, std::size_t> quadgrams;

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
            std::regex_replace(u8_text, std::regex("(\\n\\r|\\n)"), " ");
            constexpr std::size_t max_gram = 4;
            if (u8_text.size() < 4*max_gram)
                continue;
            loc::generator gen;
            loc::boundary::ssegment_index map(loc::boundary::character, u8_text.begin(), u8_text.end(), gen("de_DE.UTF-8"));
            loc::boundary::ssegment_index::iterator e = map.end()-- -- -- --;
            for (loc::boundary::ssegment_index::iterator it = map.begin(); it != e; ++it)
            {
                monograms[it->str()] += 1;
                digrams[it->str() + std::next(it)->str()] += 1;
                trigrams[it->str() + std::next(it)->str() + std::next(std::next(it))->str()] += 1;
                quadgrams[it->str() + std::next(it)->str() + std::next(std::next(it))->str() + std::next(std::next(std::next(it)))->str()] += 1;
            }
        }
    }

    using pair_type = decltype(trigrams)::value_type;
    auto max_quadgram = std::max_element(
        std::begin(quadgrams), std::end(quadgrams),
        [](const pair_type &p1, const pair_type &p2)
        {
            return p1.second < p2.second;
        });
    auto max_trigram = std::max_element(
        std::begin(trigrams), std::end(trigrams),
        [](const pair_type &p1, const pair_type &p2)
        {
            return p1.second < p2.second;
        });
    auto max_digram = std::max_element(
        std::begin(digrams), std::end(digrams),
        [](const pair_type &p1, const pair_type &p2)
        {
            return p1.second < p2.second;
        });
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
        << std::setw(6) << quadgrams.size() << " quadgrams; max. `" << max_quadgram->first << "`: " << max_quadgram->second << "\n"
        << std::setw(6) << trigrams.size() << " trigrams;  max. `" << max_trigram->first << "` : " << max_trigram->second << "\n"
        << std::setw(6) << digrams.size() << " digrams;   max. `" << max_digram->first << "`  : " << max_digram->second << "\n"
        << std::setw(6) << monograms.size() << " monograms; max. `" << max_monogram->first << "`   : " << max_monogram->second << "\n"
        << '\n';

    std::vector<ngram_t> ngrams;
    std::transform(std::begin(quadgrams), std::end(quadgrams), std::back_inserter(ngrams), [](decltype(quadgrams)::value_type it)
                   { return ngram_t{it.first, 4 * it.second}; });
    std::transform(std::begin(trigrams), std::end(trigrams), std::back_inserter(ngrams), [](decltype(trigrams)::value_type it)
                   { return ngram_t{it.first, 3 * it.second}; });
    std::transform(std::begin(digrams), std::end(digrams), std::back_inserter(ngrams), [](decltype(digrams)::value_type it)
                   { return ngram_t{it.first, 2 * it.second}; });
    std::transform(std::begin(monograms), std::end(monograms), std::back_inserter(ngrams), [](decltype(monograms)::value_type it)
                   { return ngram_t{it.first, it.second}; });
    std::sort(std::begin(ngrams), std::end(ngrams), [](decltype(ngrams)::value_type a, decltype(ngrams)::value_type b)
              { return a.count > b.count; });

    json result;
    int n = 254;
    for (auto const &ngram : ngrams)
    {
        if (n-- > 0)
            std::cout << ngram.token << ": " << ngram.count << '\n';
        result[ngram.token] = ngram.count;
    }

    std::ofstream out("result.json", std::ios::binary | std::ios::trunc);
    out << result.dump();

    return EXIT_SUCCESS;
}
