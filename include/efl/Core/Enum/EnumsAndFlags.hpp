//===- Core/Enum/EnumsAndFlags.hpp ----------------------------------===//
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
//  This file defines utilities for working with marked enums/flags.
//
//===----------------------------------------------------------------===//

#pragma once

#ifndef EFL_CORE_ENUM_ENUMSANDFLAGS_HPP
#define EFL_CORE_ENUM_ENUMSANDFLAGS_HPP

#include "Macros.hpp"
#include <efl/Core/Casts.hpp>
#include <efl/Core/_Cxx11Assert.hpp>

/**
 * Creates an explicit definition for a marked enum.
 * Allows for use of the utility functions in an unintrusive
 * manner. MUST be defined in the `efl::C` namespace.
 */
#define MEflDeclareMarked(ty, max) \
 template <> struct is_marked_enum<ty> : H::TrueType { }; \
 template <> struct largest_marked_value<ty> { \
  using UndType = underlying_type_t<ty>; \
  static constexpr UndType value = UndType(ty::max); \
 }

/**
 * Creates an explicit definition for a flagged enum.
 * Allows for use of the utility functions in an unintrusive
 * manner. MUST be defined in the `efl::C` namespace.
 */
#define MEflDeclareFlags(ty, max) \
 template <> struct is_flagged_enum<ty> : H::TrueType { }; \
 template <> struct largest_flag_bit<ty> : H::EnumBitInfo<ty> { \
  using UndType = underlying_type_t<ty>; \
  static constexpr UndType value = UndType(ty::max); \
 }

EFLI_CXPR11ASSERT_PROLOGUE_

namespace efl {
namespace C {
//=== Regular Enum Utilities ===//
template <typename E, typename = void>
struct is_marked_enum : H::FalseType { };

template <typename E>
struct is_marked_enum<E, 
  enable_if_t<sizeof(E::EFL_ENUM_MARK_END) >= 0>>
 : H::TrueType { };

template <typename E, typename = void>
struct largest_marked_value {
  COMPILE_FAILURE(E, 
    "The input type is not a marked enum. "
    "Ensure you have marked it with MEflEnumEnd.");
};

template <typename E>
struct largest_marked_value<E, 
 enable_if_t<sizeof(E::EFL_ENUM_MARK_END) >= 0>> {
  using UndType = underlying_type_t<E>;
  static constexpr UndType value =
    static_cast<UndType>(E::EFL_ENUM_MARK_END);
};

//=== Flagged Enum Utilities ===//

template <typename E, typename = void>
struct is_flagged_enum : H::FalseType { };

template <typename E>
struct is_flagged_enum<E, 
  enable_if_t<sizeof(E::EFL_ENUM_FLAG_END) >= 0>>
 : H::TrueType { };

namespace H {
  template <typename E>
  struct EnumBitInfo {
    using UndType = underlying_type_t<E>;
    static_assert(is_unsigned<UndType>::value,
      "Flags can only be set for unsigned enumerations.");
    static constexpr auto allSet_ = ~UndType(0);
    static constexpr UndType topBit_ = allSet_ ^ (allSet_ >> 1);
  };
} // namespace H

template <typename E, typename = void>
struct largest_flag_bit {
  COMPILE_FAILURE(E, 
    "The input type is not a flagged enum. "
    "Ensure you have marked it with MEflFlagEnd.");
};

template <typename E>
struct largest_flag_bit<E, 
 enable_if_t<sizeof(E::EFL_ENUM_FLAG_END) >= 0>> 
 : H::EnumBitInfo<E> {
  using UndType = underlying_type_t<E>;
  static constexpr UndType value =
    static_cast<UndType>(E::EFL_ENUM_FLAG_END);
};

//=== General Utilities ===//

template <typename E>
struct is_efl_enum : H::BoolC<
 is_marked_enum<E>::value ||
 is_flagged_enum<E>::value> { };

#if CPPVER_LEAST(14)
template <typename E>
GLOBAL bool is_marked_enum_v = 
  is_marked_enum<E>::value;

template <typename E>
GLOBAL bool is_flagged_enum_v = 
  is_flagged_enum<E>::value;

template <typename E>
GLOBAL bool is_efl_enum_v =
  is_efl_enum<E>::value;

template <typename E>
GLOBAL auto largest_marked_value_v = 
  largest_marked_value<E>::value;

template <typename E>
GLOBAL auto largest_flag_bit_v = 
  largest_flag_bit<E>::value;
#endif // Globals Check (C++14)

namespace H {
  template <typename E>
  FICONSTEXPR auto flag_mask() NOEXCEPT
   -> underlying_type_t<E> {
    using FlagI = largest_flag_bit<E>;
    return umax((FlagI::value == FlagI::topBit_) ? 
      typename FlagI::UndType(0) : (FlagI::value << 1)) - 1;
  }

  /// Checked underlying type conversion for marked enums.
  template <typename E, MEflEnableIf(
    is_marked_enum<E>::value)>
  FICONSTEXPR auto underlying(E e) NOEXCEPT
   -> underlying_type_t<E> {
    auto uval = to_underlying(e);
    EFLI_CXPR11ASSERT_(uval <= 
      largest_marked_value<E>::value);
    return uval;
  }

  /// Checked underlying type conversion for flags.
  template <typename E, MEflEnableIf(
    is_flagged_enum<E>::value)>
  FICONSTEXPR auto underlying(E e) NOEXCEPT
   -> underlying_type_t<E> {
    auto uval = to_underlying(e);
    EFLI_CXPR11ASSERT_(uval >= 0);
    EFLI_CXPR11ASSERT_(uval <= flag_mask<E>());
    return uval;
  }
} // namespace H

} // namespace C
} // namespace efl

EFLI_CXPR11ASSERT_EPILOGUE_

#endif // EFL_CORE_ENUM_ENUMSANDFLAGS_HPP
