//===- Core/Casts.hpp -----------------------------------------------===//
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
//  This file defines some casting helper functions.
//
//===----------------------------------------------------------------===//

#pragma once

#ifndef EFL_CORE_CASTS_HPP
#define EFL_CORE_CASTS_HPP

#include "Traits.hpp"
#include "Casts/Launder.hpp"
#include "Casts/Pun.hpp"

#if EFLI_BIT_CAST_ == 1
# define EFLI_PUNCAST_CXPR_ constexpr
#else
# define EFLI_PUNCAST_CXPR_
#endif

// TODO: Add isa, dyn_cast, etc. (handle casts?)

/// Uses the available "laundering" function.
#define EFL_LAUNDER(...) EFLI_LAUNDER_(__VA_ARGS__)

namespace efl {
namespace C {
/// `static_cast`s a value to `bool`.
template <typename T>
FICONSTEXPR bool bool_cast(T& t) noexcept(
 noexcept(static_cast<bool>(t))) {
  return static_cast<bool>(t);
}

/// `static_cast`s a moved value to `bool`.
template <typename T>
FICONSTEXPR bool bool_cast(T&& t) noexcept(
 noexcept(static_cast<bool>(H::cxpr_move(t)))) {
  return static_cast<bool>(H::cxpr_move(t));    
}

/// Wraps the implementation of `launder`.
/// Uses a volatile pointer wrapper as a fallback.
template <typename T>
EFLI_LAUNDERCAST_CXPR_ auto
 launder(T* t) -> H::launder_t<T> {
  return H::launder_wrap(t);
}

/// Identical to `std::launder(t)`.
template <typename T>
EFLI_LAUNDERCAST_CXPR_ auto 
 launder_cast(T* t) -> H::launder_t<T> {
  return H::launder_wrap(t);
}

template <typename T>
EFLI_LAUNDERCAST_CXPR_ auto 
 launder_cast(void* vp) -> H::launder_t<T> {
  return static_cast<H::launder_t<T>>(vp);
}

/// Converts the argument to `T*` and invokes `H::launder_wrap`.
template <typename T>
EFLI_LAUNDERCAST_CXPR_ auto 
 launder_cast(ubyte* raw_data) -> H::launder_t<T> {
  T* data = reinterpret_cast<T*>(raw_data);
  return H::launder_wrap(data);
}

/// Converts the argument to `T*` and invokes `H::launder_wrap`.
template <typename T>
EFLI_LAUNDERCAST_CXPR_ auto 
 launder_cast(std::uintptr_t raw_ptr) -> H::launder_t<T> {
  T* data = reinterpret_cast<T*>(raw_ptr);
  return H::launder_wrap(data);
}

/// VERY DANGEROUS!!! Use only when absolutely necessary
/// (eg. std::bit_cast, std::memcpy, etc. when possible).
template <typename T, typename U>
EFLI_PUNCAST_CXPR_ T pun_cast(U u) {
  MEflESAssert(std::is_trivially_copyable<T>::value &&
    std::is_trivially_copyable<U>::value);
  MEflESAssert(sizeof(T) == sizeof(U));
  return H::PunHelper<T, U>(u).get();
}

/// Casts enum to its underlying type.
template <typename E>
FICONSTEXPR underlying_type_t<E>
 underlying_cast(E e) NOEXCEPT {
  static_assert(is_enum<E>::value, 
    "underlying_cast can only be called with enums.");
  return H::to_underlying(e);
}

/// Converts signed integer to unsigned.
template <typename T, MEflEnableIf(is_signed<T>::value)>
FICONSTEXPR make_unsigned_t<T> unsigned_cast(T t) NOEXCEPT {
  return static_cast<make_unsigned_t<T>>(t);
}

/// Already unsigned, noop.
template <typename T, MEflEnableIf(is_unsigned<T>::value)>
FICONSTEXPR T unsigned_cast(T t) NOEXCEPT { return t; }

} // namespace C
} // namespace efl

#undef EFLI_LAUNDERCAST_CXPR_
#undef EFLI_PUNCAST_CXPR_

#endif // EFL_CORE_CASTS_HPP
