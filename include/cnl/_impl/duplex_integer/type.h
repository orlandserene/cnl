
//          Copyright John McFarlane 2018.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file ../LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#if !defined(CNL_IMPL_DUPLEX_INTEGER_TYPE_H)
#define CNL_IMPL_DUPLEX_INTEGER_TYPE_H

#include "../num_traits/width.h"
#include "../numbers/set_signedness.h"
#include "../power_value.h"
#include "../unreachable.h"
#include "digits.h"
#include "forward_declaration.h"
#include "numbers.h"

#include <cmath>
#include <iterator>

/// compositional numeric library
namespace cnl {
    namespace _impl {
        template<typename Integer>
        CNL_NODISCARD constexpr bool is_flushed(Integer const& value)
        {
            return value == 0 || value == static_cast<Integer>(~Integer{});
        }

        template<typename Result, typename Upper, typename Lower>
        CNL_NODISCARD constexpr auto upper_value(Upper const& upper) -> Result
        {
            return (digits<Result> <= digits<Lower>)
                         ? !is_flushed(upper)
                                 ? unreachable<Result>("overflow in narrowing conversion")
                                 : Result{}
                         : Result(sensible_left_shift<Result>(upper, digits<Lower>));
        }

        // Class duplex_integer is bigendian because this is consistent with std::pair.
        // It makes < faster but possibly makes == slower.

        template<typename Upper, typename Lower>
        class duplex_integer {
            static_assert(!numbers::signedness_v<Lower>, "Lower component must be unsigned.");

            using upper_type = Upper;
            using lower_type = Lower;

            static constexpr int lower_width = width<lower_type>;

        public:
            duplex_integer() = default;

            constexpr duplex_integer(upper_type const& u, lower_type const& l);

            template<integer Number>
            constexpr duplex_integer(Number const& n);  // NOLINT(hicpp-explicit-conversions,
                    // google-explicit-constructor)

            template<floating_point Number>
            constexpr duplex_integer(Number const& n);  // NOLINT(hicpp-explicit-conversions,
                    // google-explicit-constructor)

            CNL_NODISCARD constexpr auto upper() const -> upper_type const&
            {
                return _upper;
            }

            constexpr auto upper() -> upper_type&
            {
                return _upper;
            }

            CNL_NODISCARD constexpr auto lower() const -> lower_type const&
            {
                return _lower;
            }

            constexpr auto lower() -> lower_type&
            {
                return _lower;
            }

            CNL_NODISCARD explicit constexpr operator bool() const
            {
                return _lower || _upper;
            }

            template<integer Integer>
            CNL_NODISCARD explicit constexpr operator Integer() const
            {
                return upper_value<Integer, Upper, Lower>(_upper) | static_cast<Integer>(_lower);
            }

            template<floating_point Number>
            CNL_NODISCARD explicit constexpr operator Number() const
            {
                return static_cast<Number>(_upper) * power_value<Number, lower_width, 2>()
                     + static_cast<Number>(_lower);
            }

        private:
            // value == _upper<<lower_width + _lower
            upper_type _upper;
            lower_type _lower;
        };
    }
}

#endif  // CNL_IMPL_DUPLEX_INTEGER_TYPE_H
