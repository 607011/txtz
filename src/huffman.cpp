/*

 Copyright (c) 2024 Oliver Lau <oliver@ersatzworld.net>

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

#include <vector>
#include <queue>
#include <string>

#include "code.hpp"
#include "ngram.hpp"

namespace txtz
{

    namespace
    {
        struct huffman_node
        {
            ngram_t *ngram;
            float weight;
            huffman_node *left{nullptr};
            huffman_node *right{nullptr};

            huffman_node(ngram_t *ngram, float weight)
                : ngram(ngram), weight(weight) {}
        };

        struct weight_comparator
        {
            bool operator()(const huffman_node *const a, const huffman_node *const b)
            {
                return a->weight > b->weight; // smaller weights have higher priority
            }
        };

        void generate_codes(const huffman_node *const root, const code &prefix, std::vector<ngram_t> const &ngrams)
        {
            if (root == nullptr)
                return;
            if (root->ngram != nullptr)
            {
                root->ngram->c = prefix;
            }
            generate_codes(root->left, prefix + 0, ngrams);
            generate_codes(root->right, prefix + 1, ngrams);
        }

        void delete_tree(huffman_node *node)
        {
            if (node == nullptr)
                return;
            delete_tree(node->left);
            delete_tree(node->right);
            delete node;
        }
    }

    void huffman(std::vector<ngram_t> &ngrams)
    {
        std::priority_queue<huffman_node *, std::vector<huffman_node *>, weight_comparator> pq;

        for (auto &ngram : ngrams)
        {
            pq.push(new huffman_node(&ngram, ngram.weight));
        }

        while (pq.size() > 1)
        {
            // Extract two nodes with the lowest weight
            huffman_node *left = pq.top();
            pq.pop();
            huffman_node *right = pq.top();
            pq.pop();

            // Create a new internal node with these two nodes as children
            huffman_node *top = new huffman_node(nullptr, left->weight + right->weight);
            top->left = left;
            top->right = right;

            pq.push(top); // Add this node to the priority queue
        }

        // Now pq contains only the root of the Huffman tree
        generate_codes(pq.top(), code{}, ngrams);
        delete_tree(pq.top());
    }
}
