//===- Core/Wrapper.hpp ---------------------------------------------===//
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
//  Used for implicit initialization of other utility types like 
//  Optional and Result. Helpful for macros where the exact type isn't
//  known, but dispatch is still needed.
//
//===----------------------------------------------------------------===//

#pragma once

#ifndef EFL_CORE_WRAPPER_HPP
#define EFL_CORE_WRAPPER_HPP

#include "Traits.hpp"
#include "_Builtins.hpp"

// TODO: Use tuple?

namespace efl {
namespace C {
/// @brief A wrapper for forwarding values.
/// @tparam T The type of the value.
template <typename T>
struct Wrapper {
  Wrapper(T t) : data_(H::cxpr_forward<T>(t)) { }
  FICONSTEXPR T unwrap() { return this->data_; }
  FICONSTEXPR T value() { return this->data_; }
public:
  T data_;
};

#ifdef __cpp_deduction_guides
template <typename T>
Wrapper(T&&) -> Wrapper<T>;
#endif // Deduction guides (C++17)

/// Takes a type, wraps and returns.
template <typename T>
FICONSTEXPR Wrapper<T> 
 make_wrapper(T&& t) noexcept {
  return Wrapper<T>(H::cxpr_forward<T>(t));
}

} // namespace C
} // namespace efl

#undef // EFL_CORE_WRAPPER_HPP