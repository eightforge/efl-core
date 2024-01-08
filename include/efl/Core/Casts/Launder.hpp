//===- Core/Casts/Launder.hpp ---------------------------------------===//
//
// Copyright (C) 2024 Eightfold
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
//     limitations under the License.
//
//===----------------------------------------------------------------===//
//
//  This file implements the helpers for launder_cast, 
//  using builtins when possible.
//
//===----------------------------------------------------------------===//

#pragma once

#ifndef EFL_CORE_CASTS_LAUNDER_HPP
#define EFL_CORE_CASTS_LAUNDER_HPP

#include <efl/Core/Traits.hpp>
#include <efl/Core/_Version.hpp>

#if CPPVER_LEAST(20)
# include <new>
# define EFLI_LAUNDER_(...) ::std::launder((__VA_ARGS__))
# define EFLI_LAUNDERCAST_CXPR_ AGGRESSIVE_INLINE constexpr
# define EFLI_LAUNDER_BUILTIN_ 1
#elif __has_builtin(__builtin_launder) || defined(__GNUC__) || \
 (defined(COMPILER_MSVC) && defined(_BUILTIN_LAUNDER_SUPPORTED))
# define EFLI_LAUNDER_(...) __builtin_launder((__VA_ARGS__))
# define EFLI_LAUNDERCAST_CXPR_ ALWAYS_INLINE EFLI_CXX14_CXPR_
# define EFLI_LAUNDER_BUILTIN_ 1
#elif CPPVER_LEAST(17)
# include <new>
# define EFLI_LAUNDER_(...) ::std::launder((__VA_ARGS__))
# define EFLI_LAUNDERCAST_CXPR_ AGGRESSIVE_INLINE
#else
# define EFLI_LAUNDER_(...) ::efl::CH::to_volatile_ptr(__VA_ARGS__)
# define EFLI_LAUNDERCAST_CXPR_ AGGRESSIVE_INLINE
# define EFLI_LAUNDER_FALLBACK_ 1
#endif

#ifndef EFLI_LAUNDER_BUILTIN_
# define EFLI_LAUNDER_BUILTIN_ 0
#endif

namespace efl {
namespace C {
namespace H {
template <typename T>
struct VolatilePtr {
  ALWAYS_INLINE VolatilePtr(T* data) 
   : data_(data) { }
  
  operator T*() volatile NOEXCEPT {
    return this->data_;
  }

  T* operator->() volatile NOEXCEPT {
    return this->data_;
  }

  T& operator*() volatile NOEXCEPT {
    return *this->data_;
  }

private:
  T* data_;
};

// Converts `T*` to a `T* volatile`.
template <typename T>
ALWAYS_INLINE VolatilePtr<T>
 to_volatile_ptr(T* t) {
  return VolatilePtr<T>(t);
}

template <typename T>
EFLI_LAUNDERCAST_CXPR_ auto launder_wrap(T* t)
 -> decltype(EFLI_LAUNDER_(t)) {
  return EFLI_LAUNDER_(t);
}

template <typename T>
#ifndef EFLI_LAUNDER_FALLBACK_
using launder_t = T*;
#else
using launder_t = VolatilePtr<T>;
#endif

} // namespace H
} // namespace C
} // namespace efl

#if (EFLI_LAUNDER_BUILTIN_ == 1)
# undef EFLI_LAUNDERCAST_CXPR_
# define EFLI_LAUNDERCAST_CXPR_ HINT_INLINE EFLI_CXX14_CXPR_
#endif

#undef EFLI_LAUNDER_FALLBACK_

#endif // EFL_CORE_CASTS_LAUNDER_HPP