//===- Core/Traits/Strings.hpp --------------------------------------===//
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
//  This file implements `CharPack`, as well as string literal 
//  type conversions for C++20.
//
//===----------------------------------------------------------------===//

#pragma once

#ifndef EFL_CORE_TRAITS_STRINGS_HPP
#define EFL_CORE_TRAITS_STRINGS_HPP

#include "StdInt.hpp"

#if CPPVER_LEAST(17)
namespace efl::C::H {
template <typename T, T...CC>
struct BLitC {
  using value_type = T;
  using type = T(&)[sizeof...(CC)];
  static constexpr T data[sizeof...(CC)] { CC... };
public:
  static constexpr SzType Size() 
   NOEXCEPT { return sizeof...(CC); }
  static constexpr type Data() 
   NOEXCEPT { return BLitC::data; }
};

template <char...CC>
struct BLitC<char, CC...> {
  using value_type = char;
  using type = const char(&)[sizeof...(CC)];
  static constexpr char data[sizeof...(CC)] { CC... };
public:
  static constexpr SzType Size() 
   NOEXCEPT { return sizeof...(CC); }
  static constexpr type Data() 
   NOEXCEPT { return BLitC::data; }
  // TODO: Add conversions...
};

template <typename T>
struct BLitC<T> {
  COMPILE_FAILURE(T, 
    "Cannot generate an empty literal.");
};

# if CPPVER_LEAST(20)
namespace xx20 {
  template <SzType N>
  struct StrLit {
    CONSTEVAL StrLit(const char(&lit)[N]) 
     : StrLit(lit, MkSzSeq<N>{}) { }

    template <SzType...II>
    CONSTEVAL StrLit(const char(&lit)[N], SzSeq<II...>) 
     : data{ lit[II]... } { }
    
    FICONSTEXPR static SzType Size() 
     NOEXCEPT { return N; }

  public:
    char data[N];
  };

  template <StrLit S, SzType...II>
  FICONSTEXPR auto gen_litc(SzSeq<II...>) // NOLINT
   -> BLitC<char, S.data[II]...>;
} // namespace xx20

template <xx20::StrLit S>
using LitC = decltype(
  xx20::gen_litc<S>(
    MkSzSeq<S.Size()>{}));
# endif // C++20
} // namespace efl::C::H
#endif // C++17

#endif // EFL_CORE_TRAITS_STRINGS_HPP
