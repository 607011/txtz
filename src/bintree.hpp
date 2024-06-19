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

#ifndef __BINTREE_HPP__
#define __BINTREE_HPP__

#include <cstdlib>
#include <sstream>
#include <string>

#include "util.hpp"

template <typename CodeT, typename LengthT, typename ValueT, typename StoreT>
class bintree
{
public:
    struct node
    {
        node *left{nullptr};
        node *right{nullptr};
        ValueT value{};
    };

    ~bintree()
    {
        clear(root_);
    }

    std::string decompress(std::vector<StoreT> const &compressed)
    {
        // first entry contains length of decompressed data
        int length = int(compressed.front());
        auto it = std::next(std::begin(compressed));
        std::ostringstream oss;
        node *n = root_;
        int bit_idx = 0;
        StoreT byte = *it;
        while (length > 0)
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
                length -= int(n->value.size());
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

    /**
     * @param code
     */
    void append(CodeT code, LengthT length, ValueT const &token)
    {
        constexpr CodeT mask = (1ULL << (8 * sizeof(CodeT) - 1));
        code <<= (8 * sizeof(CodeT) - length);
        node *n = root_;
        while (length-- > 0)
        {
            if ((code & mask) == 0)
            {
                if (!n->left)
                {
                    n->left = new node();
                }
                n = n->left;
            }
            else
            {
                if (!n->right)
                {
                    n->right = new node();
                }
                n = n->right;
            }
            code <<= 1;
        }
        n->value = token;
    }

    static bool find(ValueT const &token, const node *const root)
    {
        if (root->left == nullptr && root->right == nullptr)
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

    bool has(ValueT const &token) const
    {
        return find(token, root_);
    }

    static void print(node *root, std::ostream &out)
    {
        if (root->left == nullptr && root->right == nullptr)
        {
            out << ' ' << util::escaped(root->value) << '\n';
        }
        if (root->left)
        {
            out << '0';
            print(root->left, out);
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
    node *root_{new node};

    static void clear(node *root)
    {
        if (root->left == nullptr && root->right == nullptr)
        {
            delete root;
            return;
        }
        if (root->left)
            clear(root->left);
        if (root->right)
            clear(root->right);
    }
};

#endif // __BINTREE_HPP__
