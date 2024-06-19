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

#include "code.hpp"
#include "shannon-fano.hpp"


namespace txtz
{
    code::code()
        : bitcount_(0), bits_(0) {}
    code::code(unsigned long len, code_t bits)
        : bitcount_(len), bits_(bits) {}
    code::code(code const &o)
        : bitcount_(o.bitcount_), bits_(o.bits_) {}

    void code::operator=(code const &o)
    {
        bitcount_ = o.bitcount_;
        bits_ = o.bits_;
    }

    unsigned long code::bitcount(void) const
    {
        return bitcount_;
    }

    code_t code::bits(void) const
    {
        return bits_;
    }

    void code::append(bool bit)
    {
        bits_ |= code_t(bit) << bitcount_++;
    }

    code code::operator+(bool bit)
    {
        code x{*this};
        x.append(bit);
        return x;
    }

    std::string code::str(void) const
    {
        std::ostringstream oss;
        oss << *this;
        return oss.str();
    }

}