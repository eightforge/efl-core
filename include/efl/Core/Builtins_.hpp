//===- Core/Builtins_.hpp -------------------------------------------===//
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
//  This file implements shared macros used internally.
//
//===----------------------------------------------------------------===//

#pragma once

#ifndef EFLI_CORE_BUILTINS__HPP
#define EFLI_CORE_BUILTINS__HPP

#include <CoreCommon/ConfigCache.hpp>
#if __has_include(<bits/c++config.h>)
# include <bits/c++config.h>
# define EFLI_STL_GLIBCXX_
#elif __has_include(<__config>)
# include <__config>
# define EFLI_STL_LIBCPP_
#endif

#if (__cpp_if_consteval >= 202106L)
# include <type_traits>
# define EFLI_HAS_CXPREVAL_ 1
# define EFLI_CXPREVAL_() ::std::is_constant_evaluated()
#elif defined(EFLI_STL_GLIBCXX_)
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
# define EFLI_CORE_EXPECT_(expr, val) ::efl::C::X20:: \
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
// we just don't use it. If it is fixed we will remove the check.
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
// Does this even do anything?
// It is marked noreturn, but I'm not really sure.
# define EFLI_CORE_ASSUME_(expr) \
  do { if(EFLI_EXPECT_FALSE_(!(expr))) \
    ::efl::config::unreachable(); } while(0)
#else
# define EFLI_CORE_ASSUME_(expr) (void)(0)
#endif // Assume check

// __int128 check
#ifndef EFLI_HAS_I128_
# if defined(EFLI_STL_GLIBCXX_) && \
 !defined(__STRICT_ANSI__) && \
 defined(__GLIBCXX_TYPE_INT_N_0)
// GCC supports __int128
#  define EFLI_HAS_I128_ 1
# elif defined(EFLI_STL_LIBCPP_) && \
 !defined(_LIBCPP_HAS_NO_INT128)
// Clang supports __int128
#  define EFLI_HAS_I128_ 1
# else
#  define EFLI_HAS_I128_ 0
# endif
#endif

// __float128 check
#ifndef EFLI_HAS_F128_
# if defined(EFLI_STL_GLIBCXX_) && \
 !defined(__STRICT_ANSI__) && \
 defined(_GLIBCXX_USE_FLOAT128)
// GCC supports __float128
#  define EFLI_HAS_F128_ 1
# elif defined(EFLI_STL_LIBCPP_) && \
 (defined(_M_X64) || defined(__x86_64__))
// Clang supports __float128
#  define EFLI_HAS_F128_ 1
# else
#  define EFLI_HAS_F128_ 0
# endif
#endif

//=== Implementation ===//

/// Constexpr-able assert.
#if CPPVER_LEAST(14)
# define EFLI_CXPRASSERT_(...) \
  do { EFLI_CXPRASSERT_IMPL_(bool(__VA_ARGS__)) } while(0)
#else
# define EFLI_CXPRASSERT_(...) \
  EFLI_CXPRASSERT_IMPL_(bool(__VA_ARGS__))
#endif

// Explicitly constexpr assert & constexpr-able assert impl.
#if (EFLI_HAS_CXPREVAL_ == 1) && CPPVER_LEAST(14)
# define EFLI_XCXPRASSERT_(...) \
  EFLI_CXPRASSERT_(__VA_ARGS__)
# define EFLI_CXPRASSERT_IMPL_(expr) \
  if(EFLI_EXPECT_FALSE_(EFLI_CXPREVAL_() && \
    !(expr))) EFL_UNREACHABLE(); \
  else if(EFLI_EXPECT_FALSE_( \
    !EFLI_CXPREVAL_())) assert(expr);
#elif CPPVER_LEAST(14)
# define EFLI_XCXPRASSERT_(...) (void)(0)
# define EFLI_CXPRASSERT_IMPL_(...) \
  if(EFLI_EXPECT_FALSE_(expr)) assert(false);
#else
# define EFLI_XCXPRASSERT_(...) (void)(0)
# define EFLI_CXPRASSERT_IMPL_(expr) \
  (EFLI_EXPECT_TRUE_(expr) ? (void)(0) \
    : [](){assert(false);}());
#endif

// TODO: Custom assert?
#if COMPILER_DEBUG == 1
# define EFLI_DBGASSERT_(...) \
  EFLI_CXPRASSERT_(__VA_ARGS__)
#else
# define EFLI_DBGASSERT_(...) \
  EFLI_XCXPRASSERT_(__VA_ARGS__)
#endif // Debug Check

// TODO: EFLI_QABORT_
#if defined(COMPILER_GCC) || \
 defined(COMPILER_ICC) || defined(COMPILER_CLANG)
# define EFLI_QABORT_() __builtin_trap();
#elif defined(COMPILER_MSVC)
# define EFLI_QABORT_() __debugbreak()
#else
# define EFLI_QABORT_() ::efl::config::unreachable()
#endif

#endif // EFLI_CORE_BUILTINS__HPP
