//===- Core/Casts.hpp -----------------------------------------------===//
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
//  This file defines some casting helper functions.
//
//===----------------------------------------------------------------===//

#pragma once

#ifndef EFL_CORE_CASTS_HPP
#define EFL_CORE_CASTS_HPP

#include "Traits.hpp"
#include "Casts/Pun.hpp"

#if EFLI_BIT_CAST_ == 1
# define EFLI_PUNCAST_CXPR_ constexpr
#else
# define EFLI_PUNCAST_CXPR_
#endif

namespace efl {
namespace C {
/// `static_cast`s a value to `bool`.
template <typename T>
FICONSTEXPR bool bool_cast(T& t) NOEXCEPT(
 noexcept(static_cast<bool>(t))) {
  return static_cast<bool>(t);
}

/// `static_cast`s a moved value to `bool`.
template <typename T>
FICONSTEXPR bool bool_cast(T&& t) NOEXCEPT(
 noexcept(static_cast<bool>(EFLI_CXPRMV_(t)))) {
  return static_cast<bool>(EFLI_CXPRMV_(t));    
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
} // namespace C
} // namespace efl

#undef EFLI_PUNCAST_CXPR_

#endif // EFL_CORE_CASTS_HPP
