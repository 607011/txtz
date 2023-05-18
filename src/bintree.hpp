#ifndef __BINTREE_HPP__
#define __BINTREE_HPP__

namespace codec
{

    template <typename T>
    class binary_tree
    {
    public:
        struct node
        {
            T left;
            T right;
            node(T left, T right)
                : left(left), right(right) {}
        };
    };

}

#endif // __BINTREE_HPP__
