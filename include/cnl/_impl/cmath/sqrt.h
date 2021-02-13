
//          Copyright John McFarlane 2021.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file ../LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

/// \file
/// \brief defines cnl::_impl::sqrt function for integer

#if !defined(CNL_IMPL_CMATH_SQRT_H)
#define CNL_IMPL_CMATH_SQRT_H

#include "../../limits.h"
#include "../assert.h"
#include "../num_traits/digits.h"

/// compositional numeric library
namespace cnl::_impl {
    template<_impl::integer Integer>
    [[nodiscard]] constexpr auto sqrt(Integer const& input)
    {
        // https://en.wikipedia.org/wiki/Methods_of_computing_square_roots#Binary_numeral_system_.28base_2.29
        CNL_ASSERT(input >= 0);

        auto res = +Integer{0};
        auto bit = Integer{1} << ((digits<Integer> - 1) & ~1);
        auto num = decltype(res+bit){input};

        while (bit > num) {
            bit >>= 2;
        }

        while (bit) {
            if (num >= res + bit) {
                num -= res + bit;
                res = (res >> 1) + bit;
            } else {
                res >>= 1;
            }
            bit >>= 2;
        }
        return res;
    }
}

#endif  // CNL_IMPL_CMATH_SQRT_H
