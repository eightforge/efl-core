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
# if defined(__has_builtin)
#  if __has_builtin(__make_integer_seq)
#   define ELFI_MKINTSEQ_BUILTIN_ 1   
#  endif
# endif // defined(__has_builtin)

# ifndef ELFI_MKINTSEQ_BUILTIN_
#  define ELFI_MKINTSEQ_BUILTIN_ 0
# endif
#endif

#define EFLI_SPECIALIZED_SEQ_N_ 0

/// Check if compiler has `__make_integer_seq`.
#define MEflHasMkISeqBuiltin() ELFI_MKINTSEQ_BUILTIN_
/// Get the number of specialized sequence levels.
#define MEflISeqSpecializedN() EFLI_SPECIALIZED_SEQ_N_

namespace efl {
namespace C {
namespace H {
#if CPPVER_LEAST(14)
  template <typename T, T I>
  using IntC = ::std::integral_constant<T, I>;

  template <typename T, T...II>
  using IntSeq = ::std::integer_sequence<T, II...>;

  template <SzType...II>
  using SzSeq = IntSeq<SzType, II...>;

  template <typename T, T N>
  using MkIntSeq = ::std::make_integer_sequence<T, N>;

  template <SzType N>
  using MkSzSeq = MkIntSeq<SzType, N>;

#else
  template <typename T, T I>
  struct IntC {
    static constexpr T value = I;
    using value_type = T;
    using type = IntC;
    constexpr operator value_type() 
     CNOEXCEPT { return value; }
    constexpr value_type operator()() 
     CNOEXCEPT { return value; }
  };

  template <typename T, T...II>
  struct IntSeq {
    using value_type = T;
    static constexpr SzType size() 
     NOEXCEPT { return sizeof...(II); }
  };

  template <SzType...II>
  using SzSeq = IntSeq<SzType, II...>;

# if ELFI_MKINTSEQ_BUILTIN_
  template <typename T, T N>
  using MkIntSeq = __make_integer_seq<IntSeq, T, N>;

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
    auto mk_seq_cast_(SzSeq<II...>) -> IntSeq<T, T(II)...>; // NOLINT
  } // namespace xx11

  template <SzType N>
  using MkSzSeq = xx11::mk_seq<N>;

  template <typename T, T N>
  using MkIntSeq = decltype(
    xx11::mk_seq_cast_<T>(
      MkSzSeq<SzType(N)>{}));
# endif // Has `__make_integer_seq<...>`
#endif // std::integer_sequence (C++14)

  //=== Specializations ===//

  template <bool B>
  using BoolC = IntC<bool, B>;

  using TrueType = BoolC<true>;
  using FalseType = BoolC<false>;

  template <typename...TT>
  using SzSeqFor = MkSzSeq<sizeof...(TT)>;
} // namespace H
} // namespace C
} // namespace efl

#endif // EFL_CORE_TRAITS_STDINT_HPP
