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

namespace txtz
{
    void sf_split(std::vector<token> &p, std::size_t l, std::size_t r, code c)
    {
        if (l == r)
        {
            p[l].c = c;
            return;
        }
        std::size_t pl = l;
        std::size_t pr = r;
        auto weight_left = p.at(pl).weight;
        auto weight_right = p.at(pr).weight;
        // split samples into two roughly equivalent sections
        // so that the weights of the left and right section
        // are as balanced as possible
        for (;;)
        {
            while (weight_right < weight_left && pr != pl + 1)
            {
                --pr;
                weight_right += p.at(pr).weight;
            }
            if (pr == pl + 1)
                break;
            ++pl;
            weight_left += p.at(pl).weight;
        }
        sf_split(p, l, pl, c + 0);
        sf_split(p, pr, r, c + 1);
    }

    void shannon_fano(std::vector<token> &p)
    {
        sf_split(p, 0U, p.size() - 1U, code());
    }

}