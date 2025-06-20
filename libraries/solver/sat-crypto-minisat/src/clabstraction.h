/******************************************
Copyright (C) 2009-2020 Authors of CryptoMiniSat, see AUTHORS file

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
***********************************************/

#ifndef __CL_ABSTRACTION__H__
#define __CL_ABSTRACTION__H__

#include <cstdint>

typedef uint32_t cl_abst_type;
static const int cl_abst_modulo = 29;

inline cl_abst_type abst_var(const uint32_t v)
{
    return 1UL << (v % cl_abst_modulo);
}

template <class T>
cl_abst_type calcAbstraction(const T& ps)
{
    cl_abst_type abstraction = 0;
    if (ps.size() > 50) {
        return ~((cl_abst_type)(0ULL));
    }

    for (auto l: ps)
        abstraction |= abst_var(l.var());

    return abstraction;
}

#endif //__CL_ABSTRACTION__H__
