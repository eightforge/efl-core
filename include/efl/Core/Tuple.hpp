//===- Core/Tuple.hpp -----------------------------------------------===//
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
//  The C++17 variant is based on kumi::tuple.
//
//===----------------------------------------------------------------===//

#pragma once

#ifndef EFL_CORE_TUPLE_HPP
#define EFL_CORE_TUPLE_HPP

#include <tuple>
#include "_Version.hpp"

#if CPPVER_LEAST(17)
# include "Tuple/Cxx17Base.hpp"
#else
# include "Tuple/Cxx14Base.hpp"
#endif

// TODO: Add C++20 optimizations

namespace efl {
namespace C {
namespace H {
//=== Array Backing Optimization ===//

template <IdType N, typename T>
struct TupleArr {
  static constexpr bool isArray_ = true;
  static constexpr IdType size = N;
  T data_[N];
};

template <IdType I, IdType N, typename T>
AGGRESSIVE_INLINE constexpr T& extract_leaf(
 TupleArr<N, T>& l) NOEXCEPT {
  return l.data_[N];
}

template <IdType I, IdType N, typename T>
AGGRESSIVE_INLINE constexpr T&& extract_leaf(
 TupleArr<N, T>&& l) NOEXCEPT {
  return static_cast<T&&>(l.data_[N]);
}

template <IdType I, IdType N, typename T>
AGGRESSIVE_INLINE constexpr const T& extract_leaf(
 const TupleArr<N, T>& l) NOEXCEPT {
  return l.data_[N];
}

template <IdType I, IdType N, typename T>
AGGRESSIVE_INLINE constexpr const T&& extract_leaf(
 const TupleArr<N, T>&& l) NOEXCEPT {
  return static_cast<const T&&>(l.data_[N]);
}

//=== Binding Type ===//

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

} // namespace H
} // namespace C
} // namespace efl

//=== Tuple Implementation ===//
namespace efl {
namespace C {
/**
 * A multi-inheritance based tuple for faster
 * compile times and a simpler API.
 */
template <typename...TT>
struct Tuple {
  using BaseType = H::tuple_type<TT...>;
  using SeqType = H::MkIdSeq<sizeof...(TT)>;
  static constexpr auto isArray_ = BaseType::isArray_;
  static constexpr H::SzType size = sizeof...(TT);
  
public:
  /**
   * @brief Extracts the element at `I` from the tuple.
   * @note C++20: Does not resolve if I >= Size().
   * @tparam I The element to access.
   */
  template <H::IdType I>
  EFLI_OREQUIRES_(I < sizeof...(TT))
  ALWAYS_INLINE EFLI_CXX14_CXPR_ auto operator[](H::Id<I>)&
   -> decltype(H::extract_leaf<I>(H::Decl<BaseType&>())) {
    return H::extract_leaf<I>(data_);
  }
  /// @overload
  template <H::IdType I>
  EFLI_OREQUIRES_(I < sizeof...(TT))
  ALWAYS_INLINE EFLI_CXX14_CXPR_ auto operator[](H::Id<I>)&& NOEXCEPT
   -> decltype(H::extract_leaf<I>(H::Decl<BaseType&&>())) {
    return H::extract_leaf<I>(static_cast<BaseType&&>(data_));
  }

  /// @overload
  template <H::IdType I>
  EFLI_OREQUIRES_(I < sizeof...(TT))
  FICONSTEXPR auto operator[](H::Id<I>) CONST&
   -> decltype(H::extract_leaf<I>(H::Decl<const BaseType&>())) {
    return H::extract_leaf<I>(data_);
  }

  /// @overload
  template <H::IdType I>
  EFLI_OREQUIRES_(I < sizeof...(TT))
  FICONSTEXPR auto operator[](H::Id<I>) CONST&& NOEXCEPT
   -> decltype(H::extract_leaf<I>(H::Decl<const BaseType&>())) {
    return H::extract_leaf<I>(static_cast<const BaseType&&>(data_));
  }

  ALWAYS_INLINE EFLI_CXX14_CXPR_ std::tuple<TT...> getStdTuple()& 
   NOEXCEPT { return GetAsStdTuple(*this, SeqType{}); }

  ALWAYS_INLINE EFLI_CXX14_CXPR_ std::tuple<TT...> getStdTuple()&& 
   NOEXCEPT { return GetAsStdTuple(H::cxpr_move(*this), SeqType{}); }

  FICONSTEXPR std::tuple<TT...> getStdTuple() const& 
   NOEXCEPT { return GetAsStdTuple(*this, SeqType{}); }

  FICONSTEXPR std::tuple<TT...> getStdTuple() const&&
   NOEXCEPT { return GetAsStdTuple(H::cxpr_move(*this), SeqType{}); }
  
  /// Constructs an object of type `U` from a tuple.
  template <typename U>
  constexpr U constructWithSelf() const {
    return ConstructWithTuple<U>(*this, SeqType{});
  }

  /// Returns sizeof...(TT).
  FICONSTEXPR static H::SzType Size() NOEXCEPT { return sizeof...(TT); }
  /// Returns `true` if Size() == 0.
  FICONSTEXPR static bool Empty() NOEXCEPT { return sizeof...(TT) == 0U; }
  /// Returns `true` if using array storage
  FICONSTEXPR static bool IsArray() NOEXCEPT { return Tuple::isArray_; }

private:
  template <typename Tup, H::IdType...II>
  constexpr static std::tuple<TT...>
   GetAsStdTuple(Tup&& tup, H::IdSeq<II...>) NOEXCEPT {
    return std::tuple<TT...>(
      FWD_CAST(tup[H::Id<II>{}])...);
  }

  template <typename U, typename Tup, H::IdType...II>
  FICONSTEXPR static U ConstructWithTuple(
   Tup&& tup, H::IdSeq<II...>) NOEXCEPT {
    return U(FWD_CAST(tup[H::Id<II>{}])...);
  }

public:
  BaseType data_;
};

template <H::SzType I, typename Tup>
using tuple_element_t = typename
  std::tuple_element<I, Tup>::type;

namespace H {
  template <H::SzType I, typename...TT>
  using tuple_element2_t = typename
    std::tuple_element<I, Tuple<TT...>>::type;
} // namespace H

#ifdef __cpp_deduction_guides
template <typename...TT>
Tuple(TT&&...) -> Tuple<std::decay_t<TT>...>;
#endif // Deduction guides (C++17)

/**
 * Converts a set of references (TT...)
 * into a tuple (Tuple<TT&...>).
 */
template <typename...TT>
constexpr auto tie(TT&...tt) -> Tuple<TT&...> {
  return Tuple<TT&...> {
    H::tuple_type<TT&...> { tt... }};
}

/// Perfectly forwards a set of types as a `Tuple`.
template <typename...TT>
FICONSTEXPR auto tuple_fwd(TT&&...tt) 
 -> Tuple<decltype(tt)...> {
  return Tuple<decltype(tt)...> {
    H::tuple_type<decltype(tt)...> { FWD(tt)... }};
}

/// May be used for compatibility with older versions.
template <typename...TT>
FICONSTEXPR auto make_tuple(TT&&...tt)
 -> Tuple<decay_t<TT>...> {
  return Tuple<decay_t<TT>...> {
    H::tuple_type<decay_t<TT>...> { FWD(tt)... }};
}

template <size_t I, typename...TT>
EFLI_CXX14_CXPR_ H::tuple_element2_t<I, TT...>&
 get(Tuple<TT...>& tup) noexcept {
  return H::extract_leaf<H::IdType(I)>(tup.data_);
}

template <size_t I, typename...TT>
EFLI_CXX14_CXPR_ H::tuple_element2_t<I, TT...>&&
 get(Tuple<TT...>&& tup) noexcept {
  using BaseType = typename Tuple<TT...>::BaseType;
  return H::extract_leaf<H::IdType(I)>(
    static_cast<BaseType&&>(tup.data_));
}

template <size_t I, typename...TT>
constexpr const H::tuple_element2_t<I, TT...>&
 get(const Tuple<TT...>& tup) noexcept {
  return H::extract_leaf<H::IdType(I)>(tup.data_);
}

template <size_t I, typename...TT>
constexpr const H::tuple_element2_t<I, TT...>&&
 get(const Tuple<TT...>&& tup) noexcept {
  using BaseType = typename Tuple<TT...>::BaseType;
  return H::extract_leaf<H::IdType(I)>(
    static_cast<const BaseType&&>(tup.data_));
}

} // namespace C
} // namespace efl

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
  using type = const efl::C::tuple_element_t<
    I, efl::C::Tuple<TT...>>;
};

template <typename...TT>
struct std::tuple_size<efl::C::Tuple<TT...>>
 : std::integral_constant<std::size_t, sizeof...(TT)> { };

//=== Import Get ===//
namespace std {
using efl::C::get;
} // namespace std

#endif // EFL_CORE_TUPLE_HPP
