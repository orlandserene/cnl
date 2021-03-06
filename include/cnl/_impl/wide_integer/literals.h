
//          Copyright John McFarlane 2018.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file ../LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#if !defined(CNL_IMPL_WIDE_INTEGER_LITERALS_H)
#define CNL_IMPL_WIDE_INTEGER_LITERALS_H

#include "../num_traits/rep_of.h"
#include "../unreachable.h"
#include "custom_operator.h"
#include "definition.h"

/// compositional numeric library
namespace cnl {
    namespace _impl {
        ////////////////////////////////////////////////////////////////////////////////
        // known-base integer literal parser

        template<int Base, typename ParseDigit, typename Integer>
        [[nodiscard]] constexpr auto wide_integer_parse(
                char const* s, ParseDigit parse_digit, Integer const& value) -> Integer
        {
            return *s ? wide_integer_parse<Base>(
                           s + 1, parse_digit, Integer{parse_digit(*s)} + value * Base)
                      : value;
        }

        // decimal
        [[nodiscard]] constexpr auto parse_dec_char(char c)
        {
            return (c >= '0' && c <= '9') ? c - '0' : unreachable<int>("invalid decimal digits");
        }

        template<int NumChars>
        // NOLINTNEXTLINE(cppcoreguidelines-avoid-c-arrays)
        [[nodiscard]] constexpr auto decimal_wide_integer_parse(const char (&s)[NumChars])
                -> wide_integer<(NumChars - 1) * 3322 / 1000 + 1>
        {
            using result = wide_integer<(NumChars - 1) * 3322 / 1000 + 1>;
            return result(wide_integer_parse<10>(s, parse_dec_char, rep_of_t<result>{}));
        }

        template<char... Chars>
        struct wide_integer_parser {
            [[nodiscard]] constexpr auto operator()() const
            {
                return decimal_wide_integer_parse<sizeof...(Chars) + 1>({Chars..., '\0'});
            }
        };

        // octal
        [[nodiscard]] constexpr auto parse_oct_char(char c)
        {
            return (c >= '0' && c <= '7') ? c - '0' : unreachable<int>("invalid octal digits");
        }

        template<int NumChars>
        // NOLINTNEXTLINE(cppcoreguidelines-avoid-c-arrays)
        [[nodiscard]] constexpr auto octal_wide_integer_parse(const char (&s)[NumChars])
                -> wide_integer<(NumChars - 1) * 3>
        {
            using result = wide_integer<(NumChars - 1) * 3>;
            return result(wide_integer_parse<8>(s, parse_oct_char, rep_of_t<result>{}));
        }

        template<char... Chars>
        struct wide_integer_parser<'0', Chars...> {
            [[nodiscard]] constexpr auto operator()() const
            {
                return octal_wide_integer_parse<sizeof...(Chars) + 1>({Chars..., '\0'});
            }
        };

        // binary
        [[nodiscard]] constexpr auto parse_bin_char(char c)
        {
            return (c == '0') ? 0 : (c == '1') ? 1
                                               : unreachable<int>("invalid binary digits");
        }

        template<int NumChars>
        // NOLINTNEXTLINE(cppcoreguidelines-avoid-c-arrays)
        [[nodiscard]] constexpr auto binary_wide_integer_parse(const char (&s)[NumChars])
                -> wide_integer<NumChars - 1>
        {
            using result = wide_integer<NumChars - 1>;
            return result(wide_integer_parse<2>(s, parse_bin_char, rep_of_t<result>{}));
        }

        template<char... Chars>
        struct wide_integer_parser<'0', 'B', Chars...> {
            [[nodiscard]] constexpr auto operator()() const
            {
                return binary_wide_integer_parse<sizeof...(Chars) + 1>({Chars..., '\0'});
            }
        };

        template<char... Chars>
        struct wide_integer_parser<'0', 'b', Chars...> {
            [[nodiscard]] constexpr auto operator()() const
            {
                return binary_wide_integer_parse<sizeof...(Chars) + 1>({Chars..., '\0'});
            }
        };

        // hexadecimal
        [[nodiscard]] constexpr auto parse_hex_char(char c)
        {
            return (c >= '0' && c <= '9') ? c - '0'
                 : (c >= 'a' && c <= 'z') ? c + 10 - 'a'
                 : (c >= 'A' && c <= 'Z') ? c + 10 - 'A'
                                          : unreachable<int>("invalid hexadecimal digits");
        }

        template<int NumChars>
        // NOLINTNEXTLINE(cppcoreguidelines-avoid-c-arrays)
        [[nodiscard]] constexpr auto hexadecimal_wide_integer_parse(const char (&s)[NumChars])
                -> wide_integer<(NumChars - 1) * 4>
        {
            using result = wide_integer<(NumChars - 1) * 4>;
            return result(wide_integer_parse<16>(s, parse_hex_char, rep_of_t<result>{}));
        }

        template<char... Chars>
        struct wide_integer_parser<'0', 'X', Chars...> {
            [[nodiscard]] constexpr auto operator()() const
            {
                return hexadecimal_wide_integer_parse<sizeof...(Chars) + 1>({Chars..., '\0'});
            }
        };

        template<char... Chars>
        struct wide_integer_parser<'0', 'x', Chars...> {
            [[nodiscard]] constexpr auto operator()() const
            {
                return hexadecimal_wide_integer_parse<sizeof...(Chars) + 1>({Chars..., '\0'});
            }
        };
    }

    namespace literals {
        // cnl::_impl::operator "" _wide()
        template<char... Chars>
        [[nodiscard]] constexpr auto operator"" _wide()
        {
            return _impl::wide_integer_parser<Chars...>{}();
        }
    }
}

#endif  // CNL_IMPL_WIDE_INTEGER_LITERALS_H
