/*

 Copyright (c) 2023 Oliver Lau <oliver.lau@gmail.com>

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
    void update(std::vector<ngram_t> &p, std::size_t l, std::size_t r, code c)
    {
        if (l == r)
        {
            p[l].c = c;
            return;
        }
        auto pl = l;
        auto pr = r;
        auto wl = p[pl].weight;
        auto wr = p[pr].weight;
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
        update(p, l, pl, c + 0);
        update(p, pr, r, c + 1);
    }

    void update(std::vector<ngram_t> &p)
    {
        update(p, 0, p.size() - 1, code());
    }
}