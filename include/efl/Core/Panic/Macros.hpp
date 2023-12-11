//===- Core/Panic/Macros.hpp ----------------------------------------===//
//
// Copyright (C) 2023 Eightfold
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
//  This file implements macros used for panicking, as well 
//  as some other generally useful assertion-adjacent macros.
//
//===----------------------------------------------------------------===//

#pragma once

#ifndef EFLI_CORE_PANIC_MACROS_HPP
#define EFLI_CORE_PANIC_MACROS_HPP

#include <CoreCommon/ConfigCache.hpp>
#if __has_include(<bits/c++config.h>)
# define EFLI_PANIC_GLIBCXX_ 1
#elif __has_include(<__config>)
# define EFLI_PANIC_LIBCPP_ 1
#endif

#if __cpp_if_consteval >= 202106L
# define EFLI_HAS_CXPREVAL_ 1
# define EFLI_CXPREVAL_() \
  if consteval { return true } else { return false; }
#elif CPPVER_LEAST(20)
# include <type_traits>
# define EFLI_HAS_CXPREVAL_ 1
# define EFLI_CXPREVAL_() ::std::is_constant_evaluated()
#elif defined(EFLI_PANIC_GLIBCXX_)
# include <bits/c++config.h>
# if _GLIBCXX_HAVE_IS_CONSTANT_EVALUATED
#  define EFLI_HAS_CXPREVAL_ 1
#  define EFLI_CXPREVAL_() ::std::__is_constant_evaluated()
# endif
#endif // Constant evaluation check

#ifndef EFLI_HAS_CXPREVAL_
# if __has_builtin(__builtin_is_constant_evaluated)
#  define EFLI_HAS_CXPREVAL_ 1
#  define EFLI_CXPREVAL_() __builtin_is_constant_evaluated()
# else
#  define EFLI_HAS_CXPREVAL_ 0
#  define EFLI_CXPREVAL_() false
# endif
#endif // Constant evaluation check fallback

// Optimization hints to a bunch of different compilers.
// They essentially go in decreasing order of strength.
#if __has_builtin(__builtin_expect_with_probability)
# define EFLI_CORE_EXPECT_(expr, val) \
  __builtin_expect_with_probability((expr), val, 1.0)
#elif __has_builtin(__builtin_expect)
# define EFLI_CORE_EXPECT_(expr, val) 
  __builtin_expect((expr), val)
#elif CPPVER_LEAST(20)
# define EFLI_CORE_EXPECT_(expr, val) ::efl::CH::xx20:: \
  expect_outcome_<decltype((expr)), (val)>((expr))
namespace efl::C::H::xx20 {
  template <typename T, T V>
  FICONSTEXPR T expect_outcome_(T in) {
    if(in == V) [[likely]] { return V; }
    else [[unlikely]] { return in; }
  }
} // namespace efl::C::H::xx20   
#else
# define EFLI_CORE_EXPECT_FALLBACK_ 1
# define EFLI_CORE_EXPECT_(expr, val) \
  ((expr) == (val))
#endif

/// Cast to type `ty` before passing to `expect(...)`.
#define EFLI_TEXPECT_(ty, val, expr) \
  EFLI_CORE_EXPECT_(static_cast<ty>(expr), \
    static_cast<ty>(val))
#define EFLI_EXPECT_TRUE_(...) \
  EFLI_TEXPECT_(bool, 1, (__VA_ARGS__))
#define EFLI_EXPECT_FALSE_(...) \
  EFLI_TEXPECT_(bool, 0, (__VA_ARGS__))

// Clang has optimization issues with `@llvm.assume`, so
// we just don't use it it. If it is fixed we will remove the check.
#if !defined(COMPILER_CLANG) && __has_builtin(__builtin_assume)
# define EFLI_CORE_ASSUME_(expr) \
  (__builtin_assume(static_cast<bool>(expr)))
#elif __has_builtin(__builtin_unreachable)
# define EFLI_CORE_ASSUME_(expr) \
  do { if(!(expr)) __builtin_unreachable(); } while(0)
#elif defined(COMPILER_MSVC)
# define EFLI_CORE_ASSUME_(expr) \
  do { if(!(expr)) __assume(false); } while(0)
#elif !defined(EFLI_CORE_EXPECT_FALLBACK_)
# define EFLI_CORE_ASSUME_(expr) \
  do { if(EFLI_EXPECT_FALSE_(!(expr))) \
    ::efl::config::unreachable(); } while(0)
#else
# define EFLI_CORE_ASSUME_(expr) (void)(0)
#endif // Assume check

#endif // EFLI_CORE_PANIC_MACROS_HPP
