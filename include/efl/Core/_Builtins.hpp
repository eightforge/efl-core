//===- Core/_Builtins.hpp -------------------------------------------===//
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

#ifndef EFLH_CORE_BUILTINS_HPP
#define EFLH_CORE_BUILTINS_HPP

#include <CoreCommon/ConfigCache.hpp>
#include <CoreCommon/Multithreaded.hpp>

/// Simple, `static_cast` based forwarding.
/// Prefer `std::forward` or `X11::cxpr_forward`.
#define FWD_CAST(e) static_cast<decltype(e)&&>(e)

/// Gives the compiler a hint, may help with optimization.
#define EFL_ASSUME(...) EFLI_CORE_ASSUME_((__VA_ARGS__))
/// For use inside statements, assume true.
#define EFL_LIKELY(...) EFLI_EXPECT_TRUE_((__VA_ARGS__))
/// For use inside statements, assume false.
#define EFL_UNLIKELY(...) EFLI_EXPECT_FALSE_((__VA_ARGS__))
/// Marks code as unlikely to be executed.
#define EFL_COLD_PATH EFLI_COLD_PATH_
/// Marks struct as having empty bases. Used for MSVC.
#define EFL_EMPTY_BASES EFLI_EMPTY_BASES_

//=== Implementation ===//

#if __has_include(<bits/c++config.h>)
# include <bits/c++config.h>
# define EFLI_STL_GLIBCXX_
#elif __has_include(<__config>)
# include <__config>
# define EFLI_STL_LIBCPP_
#endif

#if (__cpp_deduction_guides >= 201907L)
# define EFLI_HAS_ALIAS_UCTAD_ 1
#else
# define EFLI_HAS_ALIAS_UCTAD_ 0
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
#if __has_builtin(__builtin_expect_with_probability) || (__GNUC__ >= 9)
# define EFLI_CORE_EXPECT_(expr, val) \
  __builtin_expect_with_probability((expr), val, 1.0)
#elif __has_builtin(__builtin_expect) || defined(__GNUC__)
# define EFLI_CORE_EXPECT_(expr, val) 
  __builtin_expect((expr), val)
#elif CPPVER_LEAST(20) || (CPPVER_LEAST(11) && defined(__clang__))
# define EFLI_CORE_EXPECT_(expr, val) ::efl::C::X20:: \
  expect_outcome_<decltype((expr)), (val)>((expr))
namespace efl::C::H::xx20 {
  template <typename T, T V>
  AGGRESSIVE_INLINE constexpr T expect_outcome_(T in) {
    if(in == V) LIKELY { return V; }
    else UNLIKELY { return in; }
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
  EFLI_TEXPECT_(bool, true, (__VA_ARGS__))
#define EFLI_EXPECT_FALSE_(...) \
  EFLI_TEXPECT_(bool, false, (__VA_ARGS__))

#if CPPVER_LEAST(23)
# define EFLI_CORE_ASSUME_(expr) [[assume(expr)]]
// Clang has optimization issues with `@llvm.assume`, so
// we just don't use it. If it is fixed we will remove the check.
#elif !defined(COMPILER_CLANG) && __has_builtin(__builtin_assume)
# define EFLI_CORE_ASSUME_(expr) \
  (__builtin_assume(static_cast<bool>(expr)))
#elif __has_builtin(__builtin_unreachable) || defined(__GNUC__)
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

/// Constexpr-able assert.
#if CPPVER_LEAST(14)
# define EFLI_CXPRASSERT_(...) \
  do { EFLI_CXPRASSERT_IMPL_(bool(__VA_ARGS__)) } while(0)
#else
# define EFLI_CXPRASSERT_(...) \
  EFLI_CXPRASSERT_IMPL_(bool(__VA_ARGS__))
#endif

#if EFL_HAS_CPP_ATTRIBUTE(gnu::cold) && \
  EFL_HAS_CPP_ATTRIBUTE(gnu::noinline)
# define EFLI_COLD_PATH_ [[gnu::cold, gnu::noinline]]
#elif defined(__GNUC__)
# define EFLI_COLD_PATH_ __attribute__((cold, noinline))
#else
# define EFLI_COLD_PATH_ NOINLINE
#endif

#if defined(COMPILER_MSVC)
# define EFLI_EMPTY_BASES_ __declspec(empty_bases)
#else
# define EFLI_EMPTY_BASES_
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

#if COMPILER_DEBUG
extern "C" {
# if defined(PLATFORM_WINDOWS)
  NORETURN void __cdecl abort(void); // NOLINT 
# else // Unix?
  NORETURN void abort(void); // NOLINT 
# endif
}
# define EFLI_QABORT_() ::abort()
#else
# define EFLI_QABORT_() EFLI_TRAP_()
#endif

#if __has_builtin(__builtin_trap) || defined(__GNUC__)
# define EFLI_TRAP_() __builtin_trap();
#elif defined(COMPILER_MSVC)
# define EFLI_TRAP_() __debugbreak()
#else
# define EFLI_TRAP_() \
  *reinterpret_cast<volatile int*>(0x11) = 0
#endif

#if __has_builtin(__builtin_debugtrap)
# define EFLI_DBGTRAP_() __builtin_debugtrap()
#elif defined(COMPILER_MSVC)
# define EFLI_DBGTRAP_() __debugbreak()
#else
# define EFLI_DBGTRAP_()
#endif

#endif // EFLH_CORE_BUILTINS_HPP
