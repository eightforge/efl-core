//===- Core/Traits/Functions.hpp ------------------------------------===//
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
//  This file implements constexpr versions of functions like
//  std::move and std::forward for C++11.
//
//===----------------------------------------------------------------===//

#include "Macros.hpp"
#include <type_traits>

// constexpr std::forward pre C++14 
#define EFLI_CXPRFWD_(...) ::efl::C::H:: \
  cxpr_forward<decltype(__VA_ARGS__)>(__VA_ARGS__)
// constexpr std::move pre C++14 
#define EFLI_CXPRMV_(...) ::efl::C::H::cxpr_move(__VA_ARGS__)
// typename std::remove_reference<...>::type
#define EFLI_RMREF_(...) typename \
  ::std::remove_reference<__VA_ARGS__>::type

namespace efl {
namespace C {
namespace H {
  template <typename T>
  NODISCARD FICONSTEXPR T&& cxpr_forward(
   EFLI_RMREF_(T)& t) NOEXCEPT 
  { return static_cast<T&&>(t); }

  template <typename T>
  NODISCARD FICONSTEXPR T&& cxpr_forward(
   EFLI_RMREF_(T)&& t) NOEXCEPT {
    static_assert(!std::is_lvalue_reference<T>::value,
      "`cxpr_forward` cannot be used to "
      "convert an l-value to an r-value.");
    return static_cast<T&&>(t); 
  }

  template <typename T>
  NODISCARD FICONSTEXPR EFLI_RMREF_(T)&& 
   cxpr_move(T&& t) NOEXCEPT 
  { return static_cast<EFLI_RMREF_(T)&&>(t); }
} // namespace H
} // namespace C
} // namespace efl