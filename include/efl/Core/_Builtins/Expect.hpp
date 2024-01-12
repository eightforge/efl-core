//===- Core/_Builtins/Expect.hpp ------------------------------------===//
//
// Copyright (C) 2023-2024 Eightfold
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
//  This file implements the backend for assume/expect.
//
//===----------------------------------------------------------------===//

#pragma once

#ifndef EFLH_CORE_BUILTINS_EXPECT_HPP
#define EFLH_CORE_BUILTINS_EXPECT_HPP

#include "IConfig.hpp"

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

#endif // EFLH_CORE_BUILTINS_EXPECT_HPP
