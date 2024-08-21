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

#include <algorithm>

namespace txtz
{
    void shannon_fano(std::vector<ngram_t> &p, std::size_t l, std::size_t r, code c)
    {
        if (l == r)
        {
            p[l].c = c;
            return;
        }
        std::size_t pl = l;
        std::size_t pr = r;
        auto wl = p.at(pl).weight;
        auto wr = p.at(pr).weight;
        for (;;)
        {
            while (wr < wl && pr != pl + 1)
            {
                --pr;
                wr += p.at(pr).weight;
            }
            if (pr != pl + 1)
            {
                ++pl;
                wl += p.at(pl).weight;
            }
            else
            {
                break;
            }
        }
        shannon_fano(p, l, pl, c + 0);
        shannon_fano(p, pr, r, c + 1);
    }

    void shannon_fano(std::vector<ngram_t> &p, bool do_sort)
    {
        if (do_sort)
        {
            std::sort(std::begin(p), std::end(p), [](ngram_t const &a, ngram_t const &b)
                      { return a.weight > b.weight; });
        }
        shannon_fano(p, 0U, p.size() - 1U, code());
    }

}
