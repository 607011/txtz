#ifndef __CODE_HPP__
#define __CODE_HPP__

#include <ostream>
#include <string>


namespace txtz
{

    typedef unsigned long code_t;

    class code
    {
        unsigned long bitcount_;
        code_t bits_;

    public:
        code();
        code(unsigned long len, code_t bits);
        code(code const &o);

        void operator=(code const &o);
        unsigned long bitcount(void) const;
        code_t bits(void) const;
        void append(bool bit);
        code operator+(bool bit);
        std::string str(void) const;

        friend std::ostream &operator<<(std::ostream &os, code const &c)
        {
            auto b = c.bits_;
            for (unsigned long p = 0; p != c.bitcount_; ++p)
            {
                os << (b & 1);
                b >>= 1;
            }
            return os;
        }
    };
}

#endif
