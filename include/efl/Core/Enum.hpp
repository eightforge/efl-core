//===- Core/Enum.hpp ------------------------------------------------===//
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
//  This file defines a set of traits that can be used to interact
//  with enums. It allows for things like comparing against a 
//  set of values, checking if a value is in a specific range,
//  checking if a flag has been set, using enums as array indices, etc.
//
//===----------------------------------------------------------------===//

#pragma once

#ifndef EFL_CORE_ENUM_HPP
#define EFL_CORE_ENUM_HPP

/* 
 * Maybe implement a trait for these? 
 * Would probably look nicer...
 * 
 * eg.
 * using ESigTokenType = efl::Enum<SigTokenType>;
 * ESigTokenType::InRange<start, end>(tok.ty);
 * ESigTokenType::Matches<...>(tok.ty);
 * efl::Enum<SigTokenAttr>::HasFlag(tok.attr);
 */

/*
  template <typename T>
  FICONSTEXPR bool operator!(T r) NOEXCEPT {
    using U = underlying_type_t<T>;
    return !U(r);
  }

  template <typename T>
  FICONSTEXPR T operator~(T r) NOEXCEPT {
    using U = underlying_type_t<T>;
    return T(~U(r));
  }

  template <typename T>
  FICONSTEXPR T operator|(T l, T r) NOEXCEPT {
    using U = underlying_type_t<T>;
    return T(U(l) | U(r));
  }

  template <typename T>
  FICONSTEXPR T operator&(T l, T r) NOEXCEPT {
    using U = underlying_type_t<T>;
    return T(U(l) & U(r));
  }

  template <typename T>
  FICONSTEXPR T operator^(T l, T r) NOEXCEPT {
    using U = underlying_type_t<T>;
    return T(U(l) ^ U(r));
  }

  template <typename T>
  FICONSTEXPR T& operator|=(T& l, T r) NOEXCEPT {
    using U = underlying_type_t<T>;
    return (l = T(U(l) | U(r)));
  }

  template <typename T>
  FICONSTEXPR T& operator&=(T& l, T r) NOEXCEPT {
    using U = underlying_type_t<T>;
    return (l = T(U(l) & U(r)));
  }

  template <typename T>
  FICONSTEXPR T& operator^=(T& l, T r) NOEXCEPT {
    using U = underlying_type_t<T>;
    return (l = T(U(l) ^ U(r)));
  }
*/

#include "Enum/Flags.hpp"
#include "Enum/Overloads.hpp"

namespace efl {
namespace C {


} // namespace C
} // namespace efl

#endif // EFL_CORE_ENUM_HPP
