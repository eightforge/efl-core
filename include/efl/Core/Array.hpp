//===- Core/Array.hpp -----------------------------------------------===//
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

#pragma once

#ifndef EFL_CORE_ARRAY_HPP
#define EFL_CORE_ARRAY_HPP

#include <array>
#include "Traits/Std.hpp"
#include "_Builtins.hpp"

namespace efl {
namespace C {
/// Alias for `std::array`.
template <typename T, std::size_t N>
using Array = std::array<T, N>;

/// @brief Creates an `Array` like the C++17 deduction guide.
/// @return An array constructed from the inputs.
template <typename T, typename...TT>
constexpr Array<decay_t<T>, sizeof...(TT) + 1>
 make_array(T&& t, TT&&...tt) {
  return {{ FWD_CAST(t), FWD_CAST(tt)... }};
}

/// @brief The zero argument array overload.
/// @return For now, returns `Array<int, 0>`.
constexpr Array<int, 0> 
 make_array() { return { }; }

/// @brief Creates an array of a given type.
/// For example, `make_array_of<Str>("0", "1", "2")`
//  will return an `Array<Str, 3>`.
template <typename ArrType, typename...TT>
constexpr Array<ArrType, sizeof...(TT)>
 make_array_of(TT&&...tt) {
  return {{ FWD_CAST(tt)... }};
}

} // namespace C
} // namespace efl

#endif // EFL_CORE_ARRAY_HPP
