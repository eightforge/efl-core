//===- Core/Fundamental.hpp -----------------------------------------===//
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
//  This file defines some integral and floating point types.
//
//===----------------------------------------------------------------===//

#pragma once

#ifndef EFL_CORE_FUNDAMENTAL_HPP
#define EFL_CORE_FUNDAMENTAL_HPP

#include <cstddef>
#include <cstdint>
#include <initializer_list>
#include <CoreCommon/ConfigCache.hpp>

// TODO: Actually check for support
#ifndef EFLI_I128_ENABLED_
# if defined(COMPILER_GCC) || defined(COMPILER_CLANG)
#  define EFLI_I128_ENABLED_ 1
# else
#  define EFLI_I128_ENABLED_ 0
# endif
#endif

static_assert(CHAR_BIT == 8, "This library only works on systems with 8-bit bytes!");
static_assert(sizeof(float) == 4, "`float` is not 32-bit.");

namespace efl {
namespace C {
  /// Signed type representing a single byte.
  using ibyte = signed char;
  /// Unsigned type representing a single byte.
  /// Can be used for storage, see [intro.object].
  using ubyte = unsigned char;

  // The fundamental data types.
  using i8  = std::int8_t;
  using i16 = std::int16_t;
  using i32 = std::int32_t;
  using i64 = std::int64_t;

  using u8  = std::uint8_t;
  using u16 = std::uint16_t;
  using u32 = std::uint32_t;
  using u64 = std::uint64_t;

#if EFLI_I128_ENABLED_
  using i128 = __int128;
  using u128 = unsigned __int128;
#endif

  //=== Floating Point ===//
  
  namespace H {
    /// Deduced `double` as f64.
    template <typename T = float, 
      bool = (sizeof(T) * 2 == sizeof(double)),
      bool = (sizeof(T) * 2 == sizeof(long double))>
    struct F64DeductionHelper {
      using type = double;
    };

    /// Deduced `long double` as f64.
    template <typename T, bool B>
    struct F64DeductionHelper<T, false, B> {
      using type = long double;
    };

    /// Both `double` and `long double` are the
    /// same size as `float`, making f64 impossible.
    template <typename T>
    struct F64DeductionHelper<T, false, false> {
      enum X { f64DeductionFailure = 0 };
      static_assert(sizeof(T) == f64DeductionFailure, 
        "No floating point type larger than `float`.");
    };
  } // namespace H

  // TODO: Check for f16 and f128.
  using f32 = float;
  using f64 = typename H::F64DeductionHelper<>::type;

  //=== Extra Types ===//

  namespace H {
    struct Dummy { };
    /// Type used for generic values in TMP.
    /// TODO: Profile best integral type
    using IdType = long;
    /// Alias for std::size_t.
    using SzType = std::size_t;
    /// Alias for std::initializer_list<T>.
    template <typename T>
    using InitList = std::initializer_list<T>;
  } // namespace H

} // namespace C
} // namespace efl

#endif // EFL_CORE_FUNDAMENTAL_HPP
