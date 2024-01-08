//===- Core/Tuple/Cxx14Base.hpp -------------------------------------===//
//
// Copyright (C) 2024 Eightfold
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
//  Provides compatibility with C++14 and below.
//
//===----------------------------------------------------------------===//

#pragma once

#ifndef EFL_CORE_TUPLE_CXX14BASE_HPP
#define EFL_CORE_TUPLE_CXX14BASE_HPP

#include <efl/Core/Traits.hpp>
#include <efl/Core/_Fwd/Tuple.hpp>

//=== Basic Tuple Helpers ===//
namespace efl {
namespace C {
namespace H {
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

template <typename...TT>
struct TTupleBranch {
  using T = typename TypeSeq<TT...>::type;
  COMPILE_FAILURE(T, "You may only use up to "
    "32 values before C++17.");
};

#include "Tuple.cxx14.mac"

template <typename, typename...TT>
using TupleBranch = TTupleBranch<TT...>;

} // namespace H
} // namespace C
} // namespace efl

#endif // EFL_CORE_TUPLE_CXX14BASE_HPP
