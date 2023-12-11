//===- Core/Casts/Pun.hpp -------------------------------------------===//
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
//  This file implements the helpers for pun_cast, 
//  using builtins when possible.
//
//===----------------------------------------------------------------===//

#pragma once

#ifndef EFL_CORE_CASTS_PUN_HPP
#define EFL_CORE_CASTS_PUN_HPP

#include <efl/Core/Traits.hpp>

#if __has_builtin(__builtin_bit_cast)
# define EFLI_BIT_CAST_ 1
# define EFLI_PUNCAST_CXPR_ FICONSTEXPR
#else
# define EFLI_BIT_CAST_ 0
# define EFLI_PUNCAST_CXPR_ ALWAYS_INLINE
#endif

namespace efl {
namespace C {
namespace H {
#if EFLI_BIT_CAST_
template <typename T, typename U>
struct PunHelper {
  constexpr PunHelper(U& u) : u(u) { }
  FICONSTEXPR T get() CNOEXCEPT { 
    return __builtin_bit_cast(T, this->u); 
  }
private:
  U& u;
};
#else
template <typename T, typename U>
struct PunHelper {
  constexpr PunHelper(U u) : u(u) { }
  ALWAYS_INLINE T get() { return this->t; }
private:
  union {
    U u;
    T t;
  };
};
#endif

template <typename T>
struct PunHelper<T, T> {
  constexpr PunHelper(T& t) : t(t) { }
  FICONSTEXPR const T& get() CNOEXCEPT { 
    return t; 
  }
private:
  T& t;
};
} // namespace H
} // namespace C
} // namespace efl

#endif // EFL_CORE_CASTS_PUN_HPP
