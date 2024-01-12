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
//  This file applies `pragma once` to `efl/Config.hpp`, also
//  defining general macros/aliases used throughout the library.
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

#if CPPVER_LEAST(17)
# define EFLI_EMPTY_SASSERT_(...) static_assert(__VA_ARGS__)
#else
# define EFLI_EMPTY_SASSERT_(...) static_assert(__VA_ARGS__, \
  "Assertion `" #__VA_ARGS__ "` failed.")
#endif

namespace efl {
/// Core namespace, shrinks symbol size.
namespace C {
  namespace H {
    namespace xx11 { }
    namespace xx14 { }
    namespace xx17 { }
    namespace xx20 { }
  } // namespace H
  namespace conf = ::efl::config;
  namespace X11 = H::xx11;
  namespace X14 = H::xx14;
  namespace X17 = H::xx17;
  namespace X20 = H::xx20;
} // namespace C
/// External core alias.
namespace core = C;
namespace CH = C::H;
} // namespace efl

//=== Debug Check ===//
namespace efl {
struct IsDebugMode {
#ifdef COMPILER_DEBUG
  static constexpr bool value = COMPILER_DEBUG;
#else
  static constexpr bool value = false;
#endif
};

inline constexpr bool is_debug_mode() {
  return IsDebugMode::value;
}
} // namespace efl

#endif // EFL_CORECOMMON_CONFIGCACHE_HPP
