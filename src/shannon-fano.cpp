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
        std::size_t pl = l;
        std::size_t pr = r;
        std::size_t wl = p[pl].weight;
        std::size_t wr = p[pr].weight;
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