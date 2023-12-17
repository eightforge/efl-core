//===- Core/Traits/StdInt.hpp ---------------------------------------===//
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
//  This file implements integral_constant and integer_sequence
//  aliases, or definitions if necessary.
//
//===----------------------------------------------------------------===//

#pragma once

#ifndef EFL_CORE_TRAITS_STDINT_HPP
#define EFL_CORE_TRAITS_STDINT_HPP

#include <type_traits>
#include <utility>
#include <efl/Core/Fundamental.hpp>

#ifndef ELFI_MKINTSEQ_BUILTIN_
# if __has_builtin(__make_integer_seq)
#  define ELFI_MKINTSEQ_BUILTIN_ 1   
# else
#  define ELFI_MKINTSEQ_BUILTIN_ 0
# endif
#endif

namespace efl {
namespace C {
namespace H {
#if CPPVER_LEAST(14)
/// Alias for `std::integral_constant`.
template <typename T, T I>
using IntC = std::integral_constant<T, I>;

/// Alias for `std::integer_sequence`.
template <typename T, T...II>
using IntSeq = std::integer_sequence<T, II...>;

/// Alias for `std::index_sequence`.
template <SzType...II>
using SzSeq = IntSeq<SzType, II...>;

/// Alias for `std::make_integer_sequence`.
template <typename T, T N>
using MkIntSeq = std::make_integer_sequence<T, N>;

/// Alias for `std::make_index_sequence`.
template <SzType N>
using MkSzSeq = MkIntSeq<SzType, N>;

#else
/// Integer constant wrapper type.
template <typename T, T I>
struct IntC {
  static constexpr T value = I;
  using value_type = T;
  using type = IntC;
public:
  constexpr operator value_type() 
   const NOEXCEPT { return value; }
  constexpr value_type operator()() 
   const NOEXCEPT { return value; }
};

/// Integer sequence wrapper type.
template <typename T, T...II>
struct IntSeq {
  using value_type = T;
public:
  static constexpr SzType size() 
   NOEXCEPT { return sizeof...(II); }
};

/// Alias for `IntSeq<SzType, ...>`.
template <SzType...II>
using SzSeq = IntSeq<SzType, II...>;

// Check for `__make_integer_seq`.
# if ELFI_MKINTSEQ_BUILTIN_
/// Generates an `IntSeq` using `__make_integer_seq`.
template <typename T, T N>
using MkIntSeq = __make_integer_seq<IntSeq, T, N>;

/// Generates a `SzSeq` using `__make_integer_seq`.
template <SzType N>
using MkSzSeq = __make_integer_seq<IntSeq, SzType, N>;
# else
// For compatibility
namespace xx11 {
  template <class, class>
  struct SeqCat;

  template <SzType...II1, SzType...II2>
  struct SeqCat<SzSeq<II1...>, SzSeq<II2...>> {
    using type = SzSeq<II1..., (sizeof...(II1) + II2)...>;
  };

  template <typename T, typename U>
  using seq_cat = typename SeqCat<T, U>::type;

  template <SzType N> struct MkSeq;

#  define EFLI_SN_(n, ...) \
  template <> struct MkSeq<n> \
  { using type = SzSeq<__VA_ARGS__>; };
#  include "StdInt.seq.mac"

  template <SzType N>
  using mk_seq = typename MkSeq<N>::type;

  template <SzType N>
  struct MkSeq {
    using type = seq_cat<
      mk_seq<N / 2>, mk_seq<N - (N / 2)>>;
  };

  template <typename T, SzType...II>
  auto mk_seq_cast_(SzSeq<II...>)  // NOLINT
   -> IntSeq<T, T(II)...>;
} // namespace xx11

/// Creates a `SzSeq` using binary folding.
template <SzType N>
using MkSzSeq = xx11::mk_seq<N>;

/// Casts `SzSeq` to the appropriate type.
template <typename T, T N>
using MkIntSeq = decltype(
  xx11::mk_seq_cast_<T>(
    MkSzSeq<SzType(N)>{}));
# endif // Has `__make_integer_seq<...>`
#endif // std::integer_sequence (C++14)

//=== Specializations ===//

/// Alias for a `IntC<bool, ...>`.
template <bool B>
using BoolC = IntC<bool, B>;

using TrueType = BoolC<true>;
using FalseType = BoolC<false>;

/// Creates a `SzSeq` from `sizeof...(TT)`.
template <typename...TT>
using SzSeqFor = MkSzSeq<sizeof...(TT)>;

} // namespace H
} // namespace C
} // namespace efl

#undef ELFI_MKINTSEQ_BUILTIN_

#endif // EFL_CORE_TRAITS_STDINT_HPP
