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
#include "Traits/Std.hpp"       // <type_traits>
#include "Traits/Wrappers.hpp"  // [Type|Value]C, [Type|Value]Seq, ...
#include "Traits/Strings.hpp"   // BLitC<...>, LitC
#include "Traits/Functions.hpp" // forward, move, construct, addressof ...
#include "Traits/Invoke.hpp"    // invoke, is_invokable, invoke_result
#include "Traits/Apply.hpp"     // apply, apply_result_t
#include "Fundamental.hpp"

namespace efl {
namespace C {
#if CPPVER_LEAST(17)
using ::std::in_place_t;
using ::std::in_place_type_t;
using ::std::in_place_index_t;
using ::std::in_place;
using ::std::in_place_type;
using ::std::in_place_index;
#else

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
namespace cref_ {
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
} // namespace cref_

  template <typename T>
  using CRef = typename 
    cref_::MkCRef<T>::type;
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

/// Global `IdType` constant.
template <H::IdType I>
GLOBAL H::Id<I> Ix { };

/// Global type constant.
template <typename T>
GLOBAL H::TypeC<T> Tx { };

#if CPPVER_LEAST(17)
/// Global value constant.
template <auto V>
GLOBAL H::ValueC<decltype(V), V> Vx { };
#endif

/// Global constant for `MkIdSeq`.
template <H::IdType N>
GLOBAL H::MkIdSeq<N> Is { };

/// Global type sequence constant.
template <typename...TT>
GLOBAL H::TypeSeq<TT...> Ts { };

#if CPPVER_LEAST(17)
/// Global value sequence constant.
template <auto...VV>
GLOBAL H::AutoSeq<VV...> Vs { };
#endif

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
