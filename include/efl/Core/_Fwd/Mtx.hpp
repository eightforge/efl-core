//===- Core/Fwd_/Mtx.hpp --------------------------------------------===//
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
//  Forward declarations for Mtx/ScopedLock<...>.
//  Includes H::Mtx, ScopedLock, AdoptLock, and adopt_lock.
//
//===----------------------------------------------------------------===//

#ifndef EFL_CORE_FWD__MTX_HPP
#define EFL_CORE_FWD__MTX_HPP

#include <CoreCommon/ConfigCache.hpp>
#include <mutex>

#ifdef __cpp_lib_scoped_lock
# define EFLI_SCOPED_LOCK_(...) ::efl::C::ScopedLock \
   EFLI_UNIQUE_VAR_(_v_guard)(__VA_ARGS__)
#else
# define EFLI_SCOPED_LOCK_(...) decltype( \
   ::efl::CH::make_scoped_lock_(__VA_ARGS__)) \
   EFLI_UNIQUE_VAR_(_v_guard)(__VA_ARGS__)
#endif

namespace efl {
namespace C {
namespace H {
  template <bool>
  struct Mtx;
} // namespace H

/// Alias for `std::adopt_lock_t`.
using AdoptLock = std::adopt_lock_t;
/// Alias for `std::adopt_lock`.
GLOBAL AdoptLock adopt_lock = std::adopt_lock;

#ifdef __cpp_lib_scoped_lock
/// Alias for `std::scoped_lock`.
template <typename...TT>
using ScopedLock = std::scoped_lock<TT...>;
#else
template <typename...TT>
struct ScopedLock;
#endif // C++17 Check
}
} // namespace efl

#endif // EFL_CORE_FWD__MTX_HPP