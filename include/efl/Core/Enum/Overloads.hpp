//===- Core/Enum/Overloads.hpp --------------------------------------===//
//
// Copyright (C) 2024 Eightfold
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
//  This file defines functions/macros for using strongly typed
//  enums as if they were their underlying types.
//
//===----------------------------------------------------------------===//

#pragma once

#ifndef EFL_CORE_ENUM_OVERLOADS_HPP
#define EFL_CORE_ENUM_OVERLOADS_HPP

#include "EnumsAndFlags.hpp"
#include "Macros.hpp"

/// Pulls bitwise operators for marked enums and flags 
//  into the current namespace.
#define MEflEnableEnumOperators()       \
 using ::efl::C::H::enum_::operator!;   \
 using ::efl::C::H::enum_::operator~;   \
 using ::efl::C::H::enum_::operator|;   \
 using ::efl::C::H::enum_::operator&;   \
 using ::efl::C::H::enum_::operator^;   \
 using ::efl::C::H::enum_::operator|=;  \
 using ::efl::C::H::enum_::operator&=;  \
 using ::efl::C::H::enum_::operator^=   \

namespace efl {
namespace C {
namespace H {
namespace enum_ {
  template <typename E, MEflEnableIf(is_marked_enum<E>::value)>
  HINT_INLINE constexpr bool operator!(E r) NOEXCEPT {
    return !underlying(r);
  }

  template <typename E, MEflEnableIf(is_efl_enum<E>::value)>
  HINT_INLINE constexpr E operator~(E r) NOEXCEPT {
    return E(~underlying(r));
  }

  template <typename E, MEflEnableIf(is_efl_enum<E>::value)>
  HINT_INLINE constexpr E operator|(E l, E r) NOEXCEPT {
    return E(underlying(l) | underlying(r));
  }

  template <typename E, MEflEnableIf(is_efl_enum<E>::value)>
  HINT_INLINE constexpr E operator&(E l, E r) NOEXCEPT {
    return E(underlying(l) & underlying(r));
  }

  template <typename E, MEflEnableIf(is_efl_enum<E>::value)>
  HINT_INLINE constexpr E operator^(E l, E r) NOEXCEPT {
    return E(underlying(l) ^ underlying(r));
  }

  template <typename E, MEflEnableIf(is_efl_enum<E>::value)>
  HINT_INLINE constexpr E& operator|=(E& l, E r) NOEXCEPT {
    return (l = E(underlying(l) | underlying(r)));
  }

  template <typename E, MEflEnableIf(is_efl_enum<E>::value)>
  HINT_INLINE constexpr E& operator&=(E& l, E r) NOEXCEPT {
    return (l = E(underlying(l) & underlying(r)));
  }

  template <typename E, MEflEnableIf(is_efl_enum<E>::value)>
  HINT_INLINE constexpr E& operator^=(E& l, E r) NOEXCEPT {
    return (l = E(underlying(l) ^ underlying(r)));
  }
} // namespace enum_
} // namespace H

MEflEnableEnumOperators();

} // namespace C
} // namespace efl

#endif // EFL_CORE_ENUM_OVERLOADS_HPP
