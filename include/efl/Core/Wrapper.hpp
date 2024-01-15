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

#include "_Fwd/Tuple.hpp"
#include "_Fwd/Result.hpp"
#include "_Builtins.hpp"

namespace efl {
namespace C {
/// @brief A wrapper for forwarding values.
/// @tparam T The type of the value.
template <typename...TT>
struct Wrapper {
  constexpr Wrapper(TT...tt) 
   : data_{H::cxpr_forward<TT>(tt)...} { }
  
  template <typename U>
  constexpr operator U() const {
    return data_.template
      constructWithSelf<U>();
  }

  template <typename U, typename E>
  constexpr operator Result<U, E>() const {
    return data_.template
      constructWithSelf<Error<E>>();
  }

private:
  mutable Tuple<TT...> data_;
};

#ifdef __cpp_deduction_guides
template <typename...TT>
Wrapper(TT&&...) -> Wrapper<TT...>;
#endif // Deduction guides (C++17)

/// Takes a type, wraps and returns.
template <typename...TT>
FICONSTEXPR Wrapper<TT...> 
 make_wrapper(TT&&...tt) noexcept {
  return Wrapper<TT...>(
    H::cxpr_forward<TT>(tt)...);
}

} // namespace C
} // namespace efl

#endif // EFL_CORE_WRAPPER_HPP
