
//          Copyright John McFarlane 2018.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file ../LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#if !defined(CNL_IMPL_SCALED_INTEGER_DECLARATION_H)
#define CNL_IMPL_SCALED_INTEGER_DECLARATION_H

#include "../../limits.h"
#include "../scaled/declaration.h"
#include "../scaled/is_scaled_tag.h"

/// compositional numeric library
namespace cnl {
#if defined(__GNUG__) && !defined(__clang__)
    template<typename Rep = int, scaled_tag Scale = power<>>
#else
    template<typename Rep = int, class Scale = power<>>
#endif
    class scaled_integer;
}

#endif  // CNL_IMPL_SCALED_INTEGER_DECLARATION_H
