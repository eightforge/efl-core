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

#include "Traits/Macros.hpp"
#include "Traits/StdInt.hpp"
#include "Traits/Strings.hpp"
#include "Traits/Functions.hpp"
#include "Traits/Invoke.hpp"
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
#else
  namespace H {
    template <typename...>
    struct VoidTBase {
      using type = void;
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

  using config::bitsizeof;
#endif // Enable global traits (C++14)
} // namespace C
} // namespace efl

#endif // EFL_CORE_TRAITS_HPP
