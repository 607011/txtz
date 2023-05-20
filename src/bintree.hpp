#ifndef __BINTREE_HPP__
#define __BINTREE_HPP__

#include <cstdlib>
#include <memory>
#include <sstream>
#include <string>

#include "util.hpp"

template <typename CodeT, typename LengthT, typename ValueT>
class bintree
{
public:
    struct node
    {
        std::shared_ptr<node> left;
        std::shared_ptr<node> right;
        ValueT value;
    };

    bintree()
        : root_(std::make_shared<node>())
    {
    }

    std::string decompress(std::vector<uint8_t> const &compressed)
    {
        uint8_t length = compressed.front();
        std::vector<uint8_t>::const_iterator it = std::next(compressed.cbegin());
        std::ostringstream oss;
        std::shared_ptr<node> n = root_;
        int bit_idx = 0;
        uint8_t byte = *it;
        while (length > 0 && it != compressed.cend())
        {
            if ((byte & 0b10000000) == 0)
            {
                if (n->left)
                {
                    n = n->left;
                }
                else
                {
                    oss << n->value;
                    --length;
                    n = root_;
                }
            }
            else
            {
                if (n->right)
                {
                    n = n->right;
                }
                else
                {
                    oss << n->value;
                    --length;
                    n = root_;
                }
            }
            if (++bit_idx == 8)
            {
                bit_idx = 0;
                byte = *(++it);
            }
            else
            {
                byte <<= 1;
            }
        }
        return oss.str();
    }

    void append(CodeT code, LengthT length, std::string const &token)
    {
        constexpr CodeT mask = (1U << (8 * sizeof(CodeT) - 1));
        code <<= (8 * sizeof(CodeT) - length);
        std::shared_ptr<node> n = root_;
        while (length-- > 0)
        {
            if ((code & mask) == 0)
            {
                if (!n->left)
                {
                    n->left = std::make_shared<node>();
                }
                n = n->left;
            }
            else
            {
                if (!n->right)
                {
                    n->right = std::make_shared<node>();
                }
                n = n->right;
            }
            code <<= 1;
        }
        n->value = token;
    }

    void print(std::shared_ptr<node> root) const
    {
        if (!root->left && !root->right)
        {
            std::cout << ' ' << util::escaped(root->value) << ' ';
        }
        if (root->left)
        {
            std::cout << '0';
            print(root->left);
        }
        if (root->right)
        {
            std::cout << '1';
            print(root->right);
        }
    }

    void print() const
    {
        print(root_);
    }

private:
    std::shared_ptr<node> root_;
};

#endif // __BINTREE_HPP__
