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
#include <type_traits>
#include "_Builtins.hpp"

#undef FWD_CAST
/// Simple, `static_cast` based forwarding.
/// Prefer `std::forward` or `X11::cxpr_forward`.
#define FWD_CAST(e) static_cast<decltype(e)&&>(e)

// TODO: Actually check for __int128/__float128 support if possible.

static_assert(CHAR_BIT == 8, 
  "This library only works on systems with 8-bit bytes!");
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
using isize = typename 
  std::make_signed<std::size_t>::type;

using u8  = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using u64 = std::uint64_t;
using usize = std::size_t;

#if EFLI_HAS_I128_
using i128 = signed __int128;
using u128 = unsigned __int128;
#endif

//=== Floating Point ===//

namespace H {
  namespace fundamental_ {
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
      enum { f64DeductionFailure = 0 };
      static_assert(sizeof(T) == f64DeductionFailure, 
        "No floating point type larger than `float`.");
    };
  } // namespace fundamental_

  using f64_deduced = typename 
    fundamental_::F64DeductionHelper<>::type;
} // namespace H

// TODO: Check for f16?
using f32 = float;
using f64 = typename H::f64_deduced;

#if EFLI_HAS_F128_
using f128 = __float128;
#endif

//=== Extra Types ===//

namespace H {
  /// "Empty" type.
  struct Dummy {
    /// Identity function, returns the input.
    template <typename T>
    constexpr T&& operator[](T&& t) const NOEXCEPT { 
      return static_cast<decltype(t)&&>(t); 
    }
  };
  /// Type used for generic values in TMP.
  /// TODO: Profile best integral type
  using IdType = long;
  /// Alias for std::size_t.
  using SzType = std::size_t;
  /// Alias for std::initializer_list<T>.
  template <typename T>
  using InitList = std::initializer_list<T>;
} // namespace H

template <typename T, H::SzType N>
using array_t = T[N];

template <H::SzType N>
using carray_t = array_t<const char, N>;

namespace H {
namespace fundamental_ {
  template <typename T, SzType N>
  struct ArrayOrDummy {
    using Type = T[N];
  };

  template <typename T>
  struct ArrayOrDummy<T, 0U> {
    using Type = Dummy;
  };
} // namespace fundamental_
} // namespace H

/// Returns an array of type `T` if N > 0,
/// or `Dummy` if N == 0.
template <typename T, H::SzType N>
using array_or_dummy_t = typename
  H::fundamental_::ArrayOrDummy<T, N>::Type;
} // namespace C
} // namespace efl

#endif // EFL_CORE_FUNDAMENTAL_HPP
