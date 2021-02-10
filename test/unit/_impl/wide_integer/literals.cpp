
//          Copyright John McFarlane 2019.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file ../LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

/// \file
/// \brief tests for <cnl/_impl/wide_integer/operators.h>

#include <cnl/wide_integer.h>

#include <cnl/_impl/type_traits/identical.h>

using cnl::_impl::identical;

namespace {
    namespace test_parse {
        static_assert(identical(
                cnl::wide_integer<3>{0},
                cnl::_impl::parse<cnl::wide_integer<0>, '0'>()));

        static_assert(identical(cnl::wide_integer<3>{1}, cnl::_impl::parse<cnl::wide_integer<0>, '1'>()));

        static_assert(identical(
                cnl::wide_integer<4>{9},
                cnl::_impl::parse<cnl::wide_integer<0>, '9'>()));

        static_assert(identical(
                cnl::wide_integer<6>{10},
                cnl::_impl::parse<cnl::wide_integer<0>, '1', '0'>()));

        static_assert(identical(
                cnl::wide_integer<9>{123},
                cnl::_impl::parse<cnl::wide_integer<0>, '1', '2', '3'>()));
    }

    namespace test_literal {
        using namespace cnl::literals;

        static_assert(
                identical(
                        cnl::wide_integer<27>{0b101100101010100110101010100},
                        0b101100101010100110101010100_wide));

        static_assert(identical(cnl::wide_integer<29>{123456789}, 123456789_wide));

#if defined(__clang__) || defined(CNL_INT128_ENABLED)
        // requires that constexpr-steps is set very high
        static_assert(
                identical(
                        cnl::wide_integer<182>{1} << 180,
                        1532495540865888858358347027150309183618739122183602176_wide));
#endif

        static_assert(identical(cnl::wide_integer<15>{0x1532}, 0x1532_wide));

        static_assert(
                identical(
                        cnl::wide_integer<65>{01273465273645012763450LL},
                        01273465273645012763450_wide));
    }
}
