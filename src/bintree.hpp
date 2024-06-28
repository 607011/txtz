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

class bintree
{
public:
    struct node
    {
        node *left{nullptr};
        node *right{nullptr};
        std::string token{};
    };

    bintree(std::string stop_value) : stop_value_(stop_value) {}

    ~bintree()
    {
        clear(root_);
    }

    std::string decompress(std::vector<uint8_t> const &compressed)
    {
        auto it = std::begin(compressed);
        std::ostringstream oss;
        node *current_node = root_;
        int bit_idx = 0;
        uint8_t byte = *it;
        for (;;)
        {
            if ((byte & 0b10000000) == 0)
            {
                if (current_node->left)
                {
                    current_node = current_node->left;
                }
            }
            else
            {
                if (current_node->right)
                {
                    current_node = current_node->right;
                }
            }
            if (!current_node->left && !current_node->right)
            {
                if (current_node->token == stop_value_)
                    break;
                oss << current_node->token;
                current_node = root_;
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
    void append(unsigned long code, unsigned long length, std::string const &token)
    {
        constexpr unsigned long mask = (1ULL << (8 * sizeof(unsigned long) - 1));
        code <<= (8 * sizeof(unsigned long) - length);
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
        n->token = token;
    }

    static bool find(std::string const &token, const node *const root)
    {
        if (root->left == nullptr && root->right == nullptr)
        {
            return root->token == token;
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

    bool has(std::string const &token) const
    {
        return find(token, root_);
    }

    static void print(node *root, std::ostream &out)
    {
        if (root->left == nullptr && root->right == nullptr)
        {
            out << ' ' << util::escaped(root->token) << '\n';
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
    std::string stop_value_;

    static void clear(node *root)
    {
        if (root == nullptr)
            return;
        clear(root->left);
        clear(root->right);
        delete root;
    }
};

#endif // __BINTREE_HPP__
