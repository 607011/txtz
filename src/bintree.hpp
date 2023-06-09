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

#ifndef __BINTREE_HPP__
#define __BINTREE_HPP__

#include <cstdlib>
#include <memory>
#include <sstream>
#include <string>

#include "util.hpp"

template <typename CodeT, typename LengthT, typename ValueT, typename StoreT>
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

    std::string decompress(std::vector<StoreT> const &compressed)
    {
        StoreT length = compressed.front();
        auto it = std::next(compressed.cbegin());
        std::ostringstream oss;
        std::shared_ptr<node> n = root_;
        int bit_idx = 0;
        StoreT byte = *it;
        while (length > 0 && it != compressed.cend())
        {
            if ((byte & 0b10000000) == 0)
            {
                if (n->left)
                {
                    n = n->left;
                }
            }
            else
            {
                if (n->right)
                {
                    n = n->right;
                }
            }
            if (!n->left && !n->right)
            {
                oss << n->value;
                --length;
                n = root_;
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

    void append(CodeT code, LengthT length, ValueT const &token)
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

    bool find(ValueT const &token, std::shared_ptr<node> root)
    {
        if (!root->left && !root->right)
        {
            return root->value == token;
        }
        if (find(token, root->left))
        {
            return true;
        }
        if (find(token, root->right))
        {
            return true;
        }
        return false;
    }

    bool find(ValueT const &token)
    {
        return find(token, root_);
    }

    void print(std::shared_ptr<node> root, std::ostream &out) const
    {
        if (!root->left && !root->right)
        {
            out << ' ' << util::escaped(root->value) << '\n';
        }
        if (root->left, out)
        {
            out << '0';
            print(root->left);
        }
        if (root->right)
        {
            out << '1';
            print(root->right, out);
        }
    }

    std::string print() const
    {
        std::ostringstream oss;
        print(root_, oss);
        return oss.str();
    }

private:
    std::shared_ptr<node> root_;
};

#endif // __BINTREE_HPP__
