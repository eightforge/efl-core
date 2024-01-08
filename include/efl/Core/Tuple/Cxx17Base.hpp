//===- Core/Tuple/Cxx17Base.hpp -------------------------------------===//
//
// Copyright (C) 2023-2024 Eightfold
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
//  This file implements a tuple utilizing multiple inheritance.
//  Based on https://github.com/jfalcou/kumi.
//
//===----------------------------------------------------------------===//

#pragma once

#ifndef EFL_CORE_TUPLE_CXX17BASE_HPP
#define EFL_CORE_TUPLE_CXX17BASE_HPP

#include <efl/Core/Traits.hpp>
#include <efl/Core/_Fwd/Tuple.hpp>

//=== Basic Tuple Helpers ===//
namespace efl::C::H {
template <IdType I, typename T>
struct TupleLeaf { T data_; };

template <IdType I, typename T>
AGGRESSIVE_INLINE constexpr T& extract_leaf(
 TupleLeaf<I, T>& l) NOEXCEPT {
  return l.data_;
}

template <IdType I, typename T>
AGGRESSIVE_INLINE constexpr T&& extract_leaf(
 TupleLeaf<I, T>&& l) NOEXCEPT {
  return static_cast<T&&>(l.data_);
}

template <IdType I, typename T>
AGGRESSIVE_INLINE constexpr const T& extract_leaf(
 const TupleLeaf<I, T>& l) NOEXCEPT {
  return l.data_;
}

template <IdType I, typename T>
AGGRESSIVE_INLINE constexpr const T&& extract_leaf(
 const TupleLeaf<I, T>&& l) NOEXCEPT {
  return static_cast<const T&&>(l.data_);
}

template <typename, typename...>
struct TupleBranch;

template <IdType...II, typename...TT>
struct TupleBranch<IdSeq<II...>, TT...> : TupleLeaf<II, TT>... {
  static constexpr bool isArray_ = false;
  static constexpr auto size = sizeof...(TT);
};

} // namespace efl::C::H

#endif // EFL_CORE_TUPLE_CXX17BASE_HPP
