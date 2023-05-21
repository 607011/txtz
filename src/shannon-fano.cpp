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