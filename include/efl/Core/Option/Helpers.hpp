//===- Core/Option/Helpers.hpp --------------------------------------===//
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
//  This file implements the helper classes/functions for Option<...>.
//  std::hash<Option<...>> is not implemented here, as it would
//  including a lot of unnecessary code.
//
//===----------------------------------------------------------------===//

#pragma once

#ifndef EFL_CORE_OPTION_HELPERS_HPP
#define EFL_CORE_OPTION_HELPERS_HPP

#include "Cxx14Base.hpp"

namespace efl {
namespace C {
template <typename T>
constexpr SOption<T> make_option(T&& t) {
  return SOption<T> { FWD(t) };
}

template <typename T, typename...TT>
constexpr Option<T> make_option(TT&&...tt) {
  return Option<T> { in_place, FWD(tt)... };
}

template <typename T, typename U, typename...TT>
constexpr Option<T> make_option(H::InitList<U> il, TT&&...tt) {
  return Option<T> { in_place, il, FWD(tt)... };
}
} // namespace C
} // namespace efl

namespace std {
template <typename T, MEflEnableIf(
  std::is_move_constructible<T>::value &&
  efl::C::is_swappable<T>::value)>
#if CPPVER_LEAST(20)
constexpr
#endif
void swap(
 efl::C::Option<T>& lhs, efl::C::Option<T>& rhs)
 noexcept(noexcept(lhs.swap(rhs))) 
{ lhs.swap(rhs); }
} // namespace std

#endif // EFL_CORE_OPTION_HELPERS_HPP
