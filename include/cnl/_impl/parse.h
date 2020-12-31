
//          Copyright John McFarlane 2021.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file ../LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#if !defined(CNL_IMPL_PARSE_H)
#define CNL_IMPL_PARSE_H

#include "common.h"
#include "config.h"
#include "num_traits/digits.h"
#include "num_traits/max_digits.h"
#include "num_traits/set_digits.h"
#include "unreachable.h"

#include <cstring>
#include <numeric>
#include <tuple>
#include <utility>

namespace cnl {
    namespace _impl {

        ////////////////////////////////////////////////////////////////////////////////
        // cnl::_impl::strlen - constexpr version of std::strlen

        constexpr auto strlen(const char* str)
        {
            auto const* it{str};
            while (*it) {
                it++;
            }
            return int(it - str);
        }

        ////////////////////////////////////////////////////////////////////////////////
        // make_parse_scale_op

        template<typename Sum>
        using parse_scale_op_t = Sum (*)(Sum const&);

        constexpr auto make_parse_scale_op(int base) -> parse_scale_op_t<int64>
        {
            switch (base) {
            case 2:
                return [](int64 const& sum) {
                    return int64{(sum << 1)};
                };
            case 8:
                return [](int64 const& sum) {
                    return int64{(sum << 3)};
                };
            case 10:
                return [](int64 const& sum) {
                    return int64{(sum * 10)};
                };
            case 16:
                return [](int64 const& sum) {
                    return int64{(sum << 4)};
                };
            default:
                return unreachable<parse_scale_op_t<int64>>("unsupported number base");
            }
        }

        template<typename Sum>
        constexpr auto make_parse_scale_op_chunk32(int base) -> parse_scale_op_t<Sum>
        {
            switch (base) {
            case 2:
                return [](Sum const& sum) {
                    return Sum{(sum << 63)};
                };
            case 8:
                return [](Sum const& sum) {
                    return Sum{(sum << 63)};
                };
            case 10:
                return [](Sum const& sum) {
                    return Sum{(sum * 1'000'000'000'000'000'000)};
                };
            case 0x10:
                return [](Sum const& sum) {
                    return Sum{(sum << 60)};
                };
            default:
                return unreachable<parse_scale_op_t<Sum>>("unsupported number base");
            }
        }

        ////////////////////////////////////////////////////////////////////////////////
        // make_parse_char_op

        using parse_char_op_t = int (*)(char);

        constexpr auto make_parse_char_op_negative(int base) -> parse_char_op_t
        {
            switch (base) {
            case 2:
                return [](char c) {
                    return (c >= '0' && c <= '1') ? '0' - c : unreachable<int>("invalid binary digit");
                };
            case 8:
                return [](char c) {
                    return (c >= '0' && c <= '7') ? '0' - c : unreachable<int>("invalid octal digit");
                };
            case 10:
                return [](char c) {
                    return (c >= '0' && c <= '9') ? '0' - c : unreachable<int>("invalid decimal digit");
                };
            case 16:
                return [](char c) {
                    return (c >= '0' && c <= '9') ? '0' - c : (c >= 'a' && c <= 'z') ? ('a' - 10) - c
                                                    : (c >= 'A' && c <= 'Z')         ? ('A' - 10) - c
                                                                                     : unreachable<int>("invalid hexadecimal digit");
                };
            default:
                return unreachable<parse_char_op_t>("unsupported number base");
            }
        }

        constexpr auto make_parse_char_op_positive(int base) -> parse_char_op_t
        {
            switch (base) {
            case 2:
                return [](char c) {
                    return (c >= '0' && c <= '1') ? c - '0' : unreachable<int>("invalid binary digit");
                };
            case 8:
                return [](char c) {
                    return (c >= '0' && c <= '7') ? c - '0' : unreachable<int>("invalid octal digit");
                };
            case 10:
                return [](char c) {
                    return (c >= '0' && c <= '9') ? c - '0' : unreachable<int>("invalid decimal digit");
                };
            case 16:
                return [](char c) {
                    return (c >= '0' && c <= '9') ? c - '0' : (c >= 'a' && c <= 'z') ? c - ('a' - 10)
                                                    : (c >= 'A' && c <= 'Z')         ? c - ('A' - 10)
                                                                                     : unreachable<int>("invalid hexadecimal digit");
                };
            default:
                return unreachable<parse_char_op_t>("unsupported number base");
            };
        }

        constexpr auto make_parse_char_op(bool is_negative, int base)
        {
            return is_negative
                         ? make_parse_char_op_negative(base)
                         : make_parse_char_op_positive(base);
        }

        ////////////////////////////////////////////////////////////////////////////////
        // parse_positive/parse_negative

        struct params {
            bool is_negative;
            int base;
            int stride;
            int first_numeral;
            int num_digits;
        };

        CNL_NODISCARD constexpr auto scan_msb(
                char const* str, bool is_negative, int base, int stride, int offset, int max_num_digits)
        {
            // If most significant digit char is not too great, use a slightly narrower result type.
            // In turn, ensure that large signed numbers don't 'nudge' over to wider types.
            // E.g. 0x10000000 gets returned as 8*4-1=31 digits, not 8*4=32 digits.
            // A comprehensive solution would fully optimise width of result type.
            // E.g. 0x00000001 would report num_digits=1, not 31.
            // But this is fast, simple and avoids the pathological case.
            auto const first_digit_char{str[offset]};
            auto const first_digit{make_parse_char_op_positive(base)(first_digit_char)};
            return params{is_negative, base, stride, offset, max_num_digits - (first_digit * 2 < base)};
        }

        CNL_NODISCARD constexpr auto scan_base(char const* str, bool is_negative, int offset, int length)
        {
            if (str[offset] != '0' || offset + 1 >= length) {
                static_assert(std::numeric_limits<int32_t>::digits10 == 9);
                return scan_msb(str, is_negative, 10, 18, offset, (length * 3322 + 678) / 1000);
            }
            switch (str[offset + 1]) {
            case 'B':
            case 'b':
                return scan_msb(str, is_negative, 2, 63, offset + 2, length - 2);
            case 'X':
            case 'x':
                return scan_msb(str, is_negative, 16, 15, offset + 2, (length - 2) * 4);
            default:
                return scan_msb(str, is_negative, 8, 21, offset + 1, (length - 1) * 3);
            }
        }

        CNL_NODISCARD inline constexpr auto scan_string(char const* str, int length)
        {
            switch (str[0]) {
            case '+':
                return scan_base(str, false, 1, length - 1);
            case '-':
                return scan_base(str, true, 1, length - 1);
            default:
                return scan_base(str, false, 0, length);
            }
        }

        template<int Length>
        // NOLINTNEXTLINE(cppcoreguidelines-avoid-c-arrays)
        CNL_NODISCARD inline constexpr auto scan_string(char const (&str)[Length])
        {
            return scan_string(str, Length);
        }

        ////////////////////////////////////////////////////////////////////////////////
        // parse_string

        template<typename Result>
        CNL_NODISCARD constexpr auto parse_string(
                char const* first, char const* last, bool is_negative, int base, int stride)
        {
            if (first == last) {
                return Result{};
            }

            auto const parse_int64 = [parse_char_op = make_parse_char_op(is_negative, base),
                                      parse_scale_op = make_parse_scale_op(base)](
                                             char const* chunk_first, char const* chunk_last) {
                int64 init{};
                while (chunk_first != chunk_last) {
                    init = parse_scale_op(init) + parse_char_op(*chunk_first++);
                }
                return init;
            };

            auto const chunk_parse_scale_op{make_parse_scale_op_chunk32<Result>(base)};

            auto const* next{first + ((last - first - 1) % stride) + 1};
            Result init(parse_int64(first, next));
            while ((first = next) != last) {
                next += stride;
                init = chunk_parse_scale_op(std::move(init))
                     + parse_int64(first, next);
            }

            return init;
        }

        template<typename Result, int NumChars>
        CNL_NODISCARD constexpr auto parse_string(
                // NOLINTNEXTLINE(cppcoreguidelines-avoid-c-arrays)
                char const (&str)[NumChars], bool is_negative, int base, int stride, int first_numeral)
        {
            return parse_string<Result>(str + first_numeral, str + NumChars, is_negative, base, stride);
        }

        template<typename Result, bool IsNegative, int Base, int Stride, int FirstNumeral, char... Chars>
        CNL_NODISCARD constexpr auto parse_string()
        {
            return parse_string<Result, sizeof...(Chars)>({Chars...}, IsNegative, Base, Stride, FirstNumeral);
        }

        ////////////////////////////////////////////////////////////////////////////////
        // parse

        template<typename Narrowest>
        CNL_NODISCARD constexpr auto parse(char const* str)
        {
            auto const length{strlen(str)};
            auto params{scan_string(str, length)};
            return parse_string<Narrowest>(
                    str + params.first_numeral, str + length, params.is_negative, params.base, params.stride);
        }

        template<typename Narrowest, char... Chars>
        CNL_NODISCARD constexpr auto parse()
        {
            constexpr auto params{scan_string({Chars...})};
            constexpr auto result_digits{
                    max(digits<Narrowest>, min(params.num_digits, max_digits<Narrowest>))};
            using result_type = set_digits_t<Narrowest, result_digits>;

            return parse_string<
                    result_type, params.is_negative, params.base, params.stride, params.first_numeral, Chars...>();
        }
    }
}

#endif  // CNL_IMPL_PARSE_H
