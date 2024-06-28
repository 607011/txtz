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

#ifndef __CODE_HPP__
#define __CODE_HPP__

#include <cstddef>
#include <ostream>
#include <string>

namespace txtz
{

    typedef unsigned long code_t;

    class code
    {
        std::size_t bitcount_;
        code_t bits_;

    public:
        code(void);
        code(std::size_t len, code_t bits);
        code(code const &o);

        void operator=(code const &o);
        std::size_t bitcount(void) const;
        code_t bits(void) const;
        void append(bool bit);
        std::string str(void) const;

        friend std::ostream &operator<<(std::ostream &os, code const &c)
        {
            auto b = c.bits_;
            for (std::size_t p = 0; p != c.bitcount_; ++p)
            {
                os << (b & 1);
                b >>= 1;
            }
            return os;
        }
    };

    code operator+(code const &, int);
}

#endif
