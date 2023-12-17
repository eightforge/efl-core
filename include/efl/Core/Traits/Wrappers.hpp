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

#ifndef EFL_CORE_TRAITS_WRAPPERS_HPP
#define EFL_CORE_TRAITS_WRAPPERS_HPP

#include "StdInt.hpp"
#include <efl/Core/_Version.hpp>

namespace efl {
namespace C {
namespace H {
/// Wrapper for types.
/// Allows for deferred construction.
template <typename T>
struct TypeC {
  static constexpr SzType size_ = sizeof(T);
  using type = TypeC;
  using value_type = T;
public:
  static constexpr SzType Size() 
   NOEXCEPT { return sizeof(T); }

  /// Constructs an object of type `value_type`.
  template <typename...Args>
  static constexpr T Create(Args&&...args) {
    return T(FWD_CAST(args)...);
  }

  /// Constructs an object of type `value_type`.
  template <typename...Args>
  constexpr T operator()(Args&&...args) const {
    return T(FWD_CAST(args)...);
  }
};

/// Specialization for the type `void`.
template <>
struct TypeC<void> {
  static constexpr SzType size_ = 0;
  using type = TypeC;
  using value_type = void;
public:
  static constexpr SzType Size() 
   NOEXCEPT { return 0; }

  /// Does nothing (`void` specialization).
  template <typename...Args>
  static EFLI_CXX14_CXPR_ void Create(Args&&...) { }

  /// Does nothing (`void` specialization).
  template <typename...Args>
  EFLI_CXX14_CXPR_ void operator()(Args&&...) const { }
};

/// Wrapper for pre-`auto` values.
template <typename T, T V>
struct ValueC {
  static constexpr T value = V;
  using type = ValueC;
  using value_type = T;
public:
  static constexpr const T& Get() 
   NOEXCEPT { return value; }
  //=== STL-like functions ===//
  constexpr operator value_type() 
   const NOEXCEPT { return value; }
  constexpr value_type operator()() 
   const NOEXCEPT { return value; }
};

/// Wrapper for sequences of types.
template <typename...TT>
struct TypeSeq {
  static constexpr SzType size_ = sizeof...(TT);
  using type = TypeSeq;
public:
  static constexpr SzType size() 
   NOEXCEPT { return sizeof...(TT); }
  static constexpr SzType Size() 
   NOEXCEPT { return sizeof...(TT); }
};

/// Wrapper for sequences of similar types.
template <typename T, T...VV>
struct ValueSeq {
  static constexpr SzType size_ = sizeof...(VV);
  using Type_ = array_or_dummy_t<const T, size_>;
  using value_type = T;
  using data_type = Type_&;
  using type = ValueSeq;
  static constexpr Type_ data { VV... };
public:
  static constexpr SzType size() 
   NOEXCEPT { return sizeof...(VV); }
  static constexpr SzType Size() 
   NOEXCEPT { return sizeof...(VV); }
  static constexpr data_type Data()
   NOEXCEPT { return ValueSeq::data; }
  //=== STL-like functions ===//
  constexpr const T& operator[](SzType n)
   const NOEXCEPT { return ValueSeq::data[n]; }
};

#if CPPVER_LEAST(17)
namespace traits_ {
  template <auto...>
  struct MkValueSeq {
    using Type = ValueSeq<void>;
  };

  template <auto V, auto...VV>
  struct MkValueSeq<V, VV...> {
    using Type = ValueSeq<
      decltype(V), V, VV...>;
  };
} // namespace traits_

template <auto...VV>
using MkValueSeq = typename
  H::MkValueSeq<VV...>::Type;

template <auto V>
using AutoC = ValueC<decltype(V), V>;

/// Wrapper for sequences of arbitrary types.
template <auto...VV>
struct AutoSeq {
  static constexpr SzType size_ = sizeof...(VV);
  using value_type = T;
  using type = AutoSeq;
public:
  static constexpr SzType size() 
   NOEXCEPT { return sizeof...(VV); }
  static constexpr SzType Size() 
   NOEXCEPT { return sizeof...(VV); }
};
#endif // auto check (C++17)

} // namespace H
} // namespace C
} // namespace efl

#endif // EFL_CORE_TRAITS_WRAPPERS_HPP
