//===- ConfigCache.hpp ----------------------------------------------===//
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
//  This file just applies `pragma once` to `efl/Config.hpp`
//
//===----------------------------------------------------------------===//

#pragma once
#include <efl/Config.hpp>

#if !CPPVER_LEAST(11)
# error This library requires C++11!
#endif

#ifndef EFL_CORECOMMON_CONFIGCACHE_HPP
#define EFL_CORECOMMON_CONFIGCACHE_HPP

/// Makes things more compact :P
#define FICONSTEXPR ALWAYS_INLINE constexpr
/// Allows for "empty" `static_assert`s pre C++17.
#define MEflESAssert(...) EFLI_EMPTY_SASSERT_(__VA_ARGS__)
/// Generates a static assertion if the version is incorrect.
#define MEflCppverLeast(N) static_assert(CPPVER_LEAST(N), \
  "C++ version must be at least " #N ".")
/// Generates a static assertion if the version is incorrect.
#define MEflCppverMost(N) static_assert(CPPVER_MOST(N),   \
  "C++ version must be at most " #N ".")
/// Generates a static assertion if the version is incorrect.
#define MEflCppverMatch(N) static_assert(CPPVER_MATCH(N), \
  "C++ version must be " #N ".")

#if CPPVER_LEAST(17)
# define EFLI_EMPTY_SASSERT_(...) static_assert(__VA_ARGS__)
#else
# define EFLI_EMPTY_SASSERT_(...) static_assert(__VA_ARGS__, \
  "Assertion `" #__VA_ARGS__ "` failed.")
#endif

namespace efl {
  /// Shrinks symbol size.
  namespace C {
    namespace conf = ::efl::config;
    namespace H { }
  } // namespace C
  namespace core = C;
  namespace CH = C::H;
} // namespace efl

#endif // EFL_CORECOMMON_CONFIGCACHE_HPP
