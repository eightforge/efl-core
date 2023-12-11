//===- Core/Tuple.hpp -----------------------------------------------===//
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
//  This file "implements" type_traits, using the standard versions
//  where applicable.
//
//===----------------------------------------------------------------===//

#pragma once

#ifndef EFL_CORE_TRAITS_HPP
#define EFL_CORE_TRAITS_HPP

#include "Traits/Macros.hpp"    // MEflUnwrap, MEflEnableIf, MEflHasTrait, ...
#include "Traits/StdInt.hpp"    // [Int|Bool]C, Mk?[Int|Sz]Seq, ...
#include "Traits/Strings.hpp"   // BLitC<...>, LitC
#include "Traits/Functions.hpp" // forward, move, construct, addressof ...
#include "Traits/Invoke.hpp"    // invoke, is_invokable, invoke_result
#include "Fundamental.hpp"

namespace efl {
namespace C {
#if CPPVER_LEAST(20)
using ::std::remove_cvref;
using ::std::type_identity;
#else
template <typename T>
struct remove_cvref {
  using type = typename std::remove_reference<
    typename std::remove_cv<T>::type>::type;
};

template <typename T>
struct type_identity {
  using type = T;
};
#endif

template <typename T>
using remove_cvref_t = 
  typename remove_cvref<T>::type;

template <typename T>
using type_identity_t = T;

#if CPPVER_LEAST(17)
using ::std::void_t;
using ::std::in_place_t;
using ::std::in_place_type_t;
using ::std::in_place_index_t;
using ::std::in_place;
using ::std::in_place_type;
using ::std::in_place_index;
using ::std::is_swappable_with;
using ::std::is_swappable;
using ::std::is_nothrow_swappable_with;
using ::std::is_nothrow_swappable;
#else
namespace H {
  template <typename...>
  struct VoidTBase {
    using type = void;
  };

  namespace swap_ {
    using std::swap;

    struct TIsSwappableWith {
      template <typename T, typename U,
        typename = decltype(swap(Decl<T&>(), Decl<U&>()))>
      static TrueType Test(int); // NOLINT

      template <typename, typename>
      static FalseType Test(...); // NOLINT
    };

    struct TIsNothrowSwappableWith {
      template <typename T, typename U>
      static BoolC<NOEXCEPT(swap(
        Decl<T&>(), Decl<U&>()))> Test(int); // NOLINT

      template <typename, typename>
      static FalseType Test(...); // NOLINT
    };
  } // namespace swap_

  template <typename T, typename U>
  struct IsSwappableWith {
    using type = decltype(
      swap_::TIsSwappableWith::
      template Test<T, U>(0));
  };

  template <typename T, typename U>
  struct IsNothrowSwappableWith {
    using type = decltype(
      swap_::TIsNothrowSwappableWith::
      template Test<T, U>(0));
  };
} // namespace H

template <typename...TT>
using void_t = typename 
  H::VoidTBase<TT...>::type;

struct in_place_t { 
  explicit in_place_t() = default; 
};

template <typename T>
struct in_place_type_t { 
  explicit in_place_type_t() = default; 
};

template <H::SzType I>
struct in_place_index_t { 
  explicit in_place_index_t() = default; 
};

template <typename T, typename U>
struct is_swappable_with 
 : H::IsSwappableWith<T, U>::type { };

template <typename T>
struct is_swappable
 : H::IsSwappableWith<T, T>::type { };

template <typename T, typename U>
struct is_nothrow_swappable_with 
 : H::IsNothrowSwappableWith<T, U>::type { };

template <typename T, typename U>
struct is_nothrow_swappable
 : H::IsNothrowSwappableWith<T, T>::type { };

//=== Globals ===//

GLOBAL in_place_t in_place { };

# if CPPVER_LEAST(14)
template <typename T>
GLOBAL in_place_type_t<T> in_place_type { };

template <H::SzType I>
GLOBAL in_place_index_t<I> in_place_index { };
# endif // Globals check (C++14)
#endif

//=== Extra Stuff ===//

namespace H {
namespace H1 {
  template <typename T>
  struct MkCRef {
    using type = const T&;
  };

  template <typename T>
  struct MkCRef<const T> {
    using type = const T&;
  };

  template <typename T>
  struct MkCRef<T&> {
    using type = const T&;
  };

  template <typename T>
  struct MkCRef<const T&> {
    using type = const T&;
  };
} // namespace H1

  template <typename T>
  using CRef = typename 
    H1::MkCRef<T>::type;
} // namespace H

#if CPPVER_LEAST(14)
namespace H {
  template <IdType I>
  using Id = IntC<IdType, I>;

  template <IdType...II>
  using IdSeq = IntSeq<IdType, II...>;

  template <IdType N>
  using MkIdSeq = MkIntSeq<IdType, N>;
} // namespace H

template <H::IdType I>
GLOBAL H::Id<I> Ix { };

template <H::IdType N>
GLOBAL H::MkIdSeq<N> Is { };

# if CPPVER_LEAST(20)
namespace xx20 { 
  using ::efl::C::H::xx20::StrLit; 
} // namespace xx20

template <xx20::StrLit S>
GLOBAL H::LitC<S> Ls { };
# endif // Literal interning (C++20)

using conf::bitsizeof;
#endif // Enable global traits (C++14)
} // namespace C
} // namespace efl

#endif // EFL_CORE_TRAITS_HPP
