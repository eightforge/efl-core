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
//  This file implements a tuple utilizing multiple inheritance.
//  Based on https://github.com/jfalcou/kumi.
//
//===----------------------------------------------------------------===//

#pragma once

#ifndef EFL_CORE_TUPLE_HPP
#define EFL_CORE_TUPLE_HPP

#include "Traits.hpp"

#if !CPPVER_LEAST(17)
# error C++17 required for this library!
#endif

//=== Basic Tuple Helpers ===//
namespace efl::C::H {
template <IdType I, typename T>
struct TupleLeaf { T data_; };

template <IdType I, typename T>
FICONSTEXPR T& extract_leaf(
 TupleLeaf<I, T>& l) NOEXCEPT {
  return l.data_;
}

template <IdType I, typename T>
FICONSTEXPR T&& extract_leaf(
 TupleLeaf<I, T>&& l) NOEXCEPT {
  return static_cast<T&&>(l.data_);
}

template <IdType I, typename T>
FICONSTEXPR const T& extract_leaf(
 const TupleLeaf<I, T>& l) NOEXCEPT {
  return l.data_;
}

template <IdType I, typename T>
FICONSTEXPR const T&& extract_leaf(
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

//=== Array Backing Optimization ===//
namespace efl::C::H {
template <IdType N, typename T>
struct TupleArr {
  static constexpr bool isArray_ = true;
  static constexpr IdType size = N;
  T data_[N];
};

template <IdType I, IdType N, typename T>
FICONSTEXPR T& extract_leaf(
 TupleArr<N, T>& l) NOEXCEPT {
  return l.data_[N];
}

template <IdType I, IdType N, typename T>
FICONSTEXPR T&& extract_leaf(
 TupleArr<N, T>&& l) NOEXCEPT {
  return static_cast<T&&>(l.data_[N]);
}

template <IdType I, IdType N, typename T>
FICONSTEXPR const T& extract_leaf(
 const TupleArr<N, T>& l) NOEXCEPT {
  return l.data_[N];
}

template <IdType I, IdType N, typename T>
FICONSTEXPR const T&& extract_leaf(
 const TupleArr<N, T>&& l) NOEXCEPT {
  return static_cast<const T&&>(l.data_[N]);
}
} // namespace efl::C::H

//=== Binding Type ===//
namespace efl::C::H {
template <typename IIs, typename...TT>
struct TupleSelector {
  using type = TupleBranch<IIs, TT...>;
  static constexpr bool isArray_ = type::isArray_;
};

#if CPPVER_LEAST(20)
template <typename T, typename...TT>
concept uniform_types = 
  (true && ... && std::same_as<T, TT>);

template <typename T, typename...TT>
concept array_compatible = 
  !std::is_reference_v<T> && 
  uniform_types<T, TT...>;

template <IdType...II, typename Tx, typename Ty, typename...TT>
requires(array_compatible<Tx, Ty, TT...>)
struct TupleSelector<IdSeq<II...>, Tx, Ty, TT...> {
  using type = TupleArr<IdType(sizeof...(II)), Tx>;
};
#endif

template <typename...TT>
using tuple_type = typename TupleSelector<
  MkIdSeq<sizeof...(TT)>, TT...>::type;
} // namespace efl::C::H

//=== Tuple Implementation ===//
namespace efl::C {
/**
 * A multi-inheritance based tuple for faster
 * compile times and a simpler API.
 */
template <typename...TT>
struct Tuple {
  using baseType_ = H::tuple_type<TT...>;
  static constexpr auto isArray_ = baseType_::isArray_;
  static constexpr H::SzType size = sizeof...(TT);
  
public:
  /**
   * @brief Extracts the element at `I` from the tuple.
   * @note C++20: Does not resolve if I >= Size().
   * @tparam I The element to access.
   */
  template <H::IdType I>
  EFLI_OREQUIRES_(I < sizeof...(TT))
  FICONSTEXPR decltype(auto) operator[](H::Id<I>)& {
    return H::extract_leaf<I>(data_);
  }
  /// @overload
  template <H::IdType I>
  EFLI_OREQUIRES_(I < sizeof...(TT))
  FICONSTEXPR decltype(auto) operator[](H::Id<I>)&& NOEXCEPT {
    return H::extract_leaf<I>(static_cast<baseType_&&>(data_));
  }

  /// @overload
  template <H::IdType I>
  EFLI_OREQUIRES_(I < sizeof...(TT))
  FICONSTEXPR decltype(auto) operator[](H::Id<I>) CONST& {
    return H::extract_leaf<I>(data_);
  }

  /// @overload
  template <H::IdType I>
  EFLI_OREQUIRES_(I < sizeof...(TT))
  FICONSTEXPR decltype(auto) operator[](H::Id<I>) CONST&& NOEXCEPT {
    return H::extract_leaf<I>(static_cast<const baseType_&&>(data_));
  }

  /// Returns sizeof...(TT).
  FICONSTEXPR static H::SzType Size() NOEXCEPT { return sizeof...(TT); }
  /// Returns `true` if Size() == 0.
  FICONSTEXPR static bool Empty() NOEXCEPT { return sizeof...(TT) == 0U; }
  /// Returns `true` if using array storage
  FICONSTEXPR static bool IsArray() NOEXCEPT { return Tuple::isArray_; }

public:
  baseType_ data_;
};

template <typename...TT>
Tuple(TT&&...) -> Tuple<std::decay_t<TT>...>;

/**
 * Converts a set of references (TT...)
 * into a tuple (Tuple<TT&...>).
 */
template <typename...TT>
FICONSTEXPR auto tie(TT&...tt) -> Tuple<TT&...> {
  return { tt... };
}

/// Perfectly forwards a set of types as a `Tuple`.
template <typename...TT>
FICONSTEXPR auto tuple_fwd(TT&&...tt) 
 -> Tuple<decltype(tt)...> {
  return { FWD(tt)... };
}
} // namespace efl::C

//=== Std Traits ===//
template <std::size_t I, typename T, typename...TT>
struct std::tuple_element<I, efl::C::Tuple<T, TT...>>
 : std::tuple_element<I - 1, efl::C::Tuple<TT...>> { };

template <typename T, typename...TT>
struct std::tuple_element<0, efl::C::Tuple<T, TT...>> {
  using type = T;
};

template <std::size_t I, typename...TT>
struct std::tuple_element<I, const efl::C::Tuple<TT...>> {
  using type = const std::tuple_element_t<
    I, efl::C::Tuple<TT...>>;
};

template <typename...TT>
struct std::tuple_size<efl::C::Tuple<TT...>>
 : std::integral_constant<std::size_t, sizeof...(TT)> { };

#endif // EFL_CORE_TUPLE_HPP