//===- Core/Option/Compare.hpp --------------------------------------===//
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
//  This file implements the comparison functions for Option<...>.
//
//===----------------------------------------------------------------===//

#pragma once

#ifndef EFL_CORE_OPTION_COMPARE_HPP
#define EFL_CORE_OPTION_COMPARE_HPP

#if __cpp_lib_three_way_comparison
# include <compare>
#endif // operator <=> (C++20)

#include <efl/Core/Casts.hpp>
#include "Cxx14Base.hpp"

#define EFLI_OP_(sym) decltype( \
 std::declval<T>() sym std::declval<U>())

namespace efl {
namespace C {
namespace ops_ {
namespace H {
  /// Enabled if operator is bool convertible.
  template <typename T>
  using option_vop = enable_if_t<
    std::is_convertible<T, bool>::value, bool>;
  
  /// `true` if type is `Option<...>`.
  template <typename T>
  struct IsOption : ::efl::CH::FalseType { };

  /// `true` if type is `Option<...>`.
  template <typename T>
  struct IsOption<Option<T>> : ::efl::CH::TrueType { };

#  ifdef __cpp_concepts
  template <typename T>
  concept is_option = IsOption<T>::value;
#  endif
} // namespace H

template <typename T, typename U>
constexpr auto operator==(
 const Option<T>& t, const Option<U>& u) NOEXCEPT
 -> H::option_vop<EFLI_OP_(==)> {
  return (bool_cast(t) == bool_cast(u)) &&
    (!t || *t == *u);
}

template <typename T, typename U>
constexpr auto operator!=(
 const Option<T>& t, const Option<U>& u) NOEXCEPT
 -> H::option_vop<EFLI_OP_(!=)> {
  return (bool_cast(t) != bool_cast(u)) &&
    (bool_cast(t) || *t != *u);
}

template <typename T, typename U>
constexpr auto operator<(
 const Option<T>& t, const Option<U>& u) NOEXCEPT
 -> H::option_vop<EFLI_OP_(<)> {
  return bool_cast(u) && (!t || *t < *u);
}

template <typename T, typename U>
constexpr auto operator>(
 const Option<T>& t, const Option<U>& u) NOEXCEPT
 -> H::option_vop<EFLI_OP_(>)> {
  return bool_cast(t) && (!u || *t > *u);
}

template <typename T, typename U>
constexpr auto operator<=(
 const Option<T>& t, const Option<U>& u) NOEXCEPT
 -> H::option_vop<EFLI_OP_(<=)> {
  return !t || (bool_cast(u) && *t <= *u);
}

template <typename T, typename U>
constexpr auto operator>=(
 const Option<T>& t, const Option<U>& u) NOEXCEPT
 -> H::option_vop<EFLI_OP_(>=)> {
  return !u || (bool_cast(t) && *t >= *u);
}

#ifdef __cpp_lib_three_way_comparison
template <typename T, 
  std::three_way_comparable_with<T> U>
constexpr std::compare_three_way_result_t<T, U> operator<=>(
 const Option<T>& t, const Option<U>& u) NOEXCEPT {
  return t && u ? *t <=> *u : bool_cast(t) <=> bool_cast(u);
}
#endif

//=== Arbitrary Types ===//

template <typename T, typename U>
constexpr auto operator==(
 const Option<T>& t, const U& u) NOEXCEPT
 -> H::option_vop<EFLI_OP_(==)> 
{ return t && *t == u; }

template <typename T, typename U>
constexpr auto operator==(
 const U& u, const Option<T>& t) NOEXCEPT
 -> H::option_vop<EFLI_OP_(==)> 
{ return t && u == *t; }

template <typename T, typename U>
constexpr auto operator!=(
 const Option<T>& t, const U& u) NOEXCEPT
 -> H::option_vop<EFLI_OP_(!=)> 
{ return !t || *t != u; }

template <typename T, typename U>
constexpr auto operator!=(
 const U& u, const Option<T>& t) NOEXCEPT
 -> H::option_vop<EFLI_OP_(!=)> 
{ return !t || u != *t; }


template <typename T, typename U>
constexpr auto operator<(
 const Option<T>& t, const U& u) NOEXCEPT
 -> H::option_vop<EFLI_OP_(<)> 
{ return !t || *t < u; }

template <typename T, typename U>
constexpr auto operator<(
 const U& u, const Option<T>& t) NOEXCEPT
 -> H::option_vop<EFLI_OP_(<)> 
{ return t && u < *t; }

template <typename T, typename U>
constexpr auto operator>(
 const Option<T>& t, const U& u) NOEXCEPT
 -> H::option_vop<EFLI_OP_(>)> 
{ return t && *t > u; }

template <typename T, typename U>
constexpr auto operator>(
 const U& u, const Option<T>& t) NOEXCEPT
 -> H::option_vop<EFLI_OP_(>)> 
{ return !t || u > *t; }

template <typename T, typename U>
constexpr auto operator<=(
 const Option<T>& t, const U& u) NOEXCEPT
 -> H::option_vop<EFLI_OP_(<=)> 
{ return !t || *t <= u; }

template <typename T, typename U>
constexpr auto operator<=(
 const U& u, const Option<T>& t) NOEXCEPT
 -> H::option_vop<EFLI_OP_(<=)> 
{ return t && u <= *t; }

template <typename T, typename U>
constexpr auto operator>=(
 const Option<T>& t, const U& u) NOEXCEPT
 -> H::option_vop<EFLI_OP_(>=)> {
  return t && *t >= u;
}

template <typename T, typename U>
constexpr auto operator>=(
 const U& u, const Option<T>& t) NOEXCEPT
 -> H::option_vop<EFLI_OP_(>=)> {
  return !t || u >= *t;
}

#ifdef __cpp_lib_three_way_comparison
template <typename T, typename U>
  requires(!H::is_option<U>)
    && std::three_way_comparable_with<T, U>
  constexpr std::compare_three_way_result_t<T, U>
   operator<=>(const Option<T>& t, const U& u) { 
    return bool_cast(t) ? *t <=> u : 
      std::strong_ordering::less; 
  }
#endif

//=== NullOpt ===//

#ifdef __cpp_lib_three_way_comparison
template <typename T>
constexpr std::strong_ordering operator<=>(
 const Option<T>& t, NullOpt) noexcept
{ return bool_cast(t) <=> false; }
#else

template <typename T>
constexpr bool operator==(
 const Option<T>& t, NullOpt) NOEXCEPT 
{ return !t; }

template <typename T>
constexpr bool operator==(
 NullOpt, const Option<T>& t) NOEXCEPT 
{ return !t; }

template <typename T>
constexpr bool operator!=(
 const Option<T>& t, NullOpt) NOEXCEPT 
{ return bool_cast(t); }

template <typename T>
constexpr bool operator!=(
 NullOpt, const Option<T>& t) NOEXCEPT 
{ return bool_cast(t); }

template <typename T>
constexpr bool operator<(
 const Option<T>&, NullOpt) NOEXCEPT 
{ return false; }

template <typename T>
constexpr bool operator<(
 NullOpt, const Option<T>& t) NOEXCEPT 
{ return bool_cast(t); }

template <typename T>
constexpr bool operator>(
 const Option<T>& t, NullOpt) NOEXCEPT 
{ return bool_cast(t); }

template <typename T>
constexpr bool operator>(
 NullOpt, const Option<T>&) NOEXCEPT 
{ return false; }

template <typename T>
constexpr bool operator<=(
 const Option<T>& t, NullOpt) NOEXCEPT 
{ return !t; }

template <typename T>
constexpr bool operator<=(
 NullOpt, const Option<T>&) NOEXCEPT 
{ return true; }

template <typename T>
constexpr bool operator>=(
 const Option<T>&, NullOpt) NOEXCEPT 
{ return true; }

template <typename T>
constexpr bool operator>=(
 NullOpt, const Option<T>& t) NOEXCEPT 
{ return !t; }
#endif
} // namespace ops_
} // namespace C
} // namespace efl

#undef EFLI_OP_

using namespace efl::C::ops_;

#endif // EFL_CORE_OPTION_COMPARE_HPP
