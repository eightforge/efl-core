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
//  std::move, std::forward, and std::construct_at for C++11.
//
//===----------------------------------------------------------------===//

#pragma once

#ifndef EFL_CORE_TRAITS_FUNCTIONS_HPP
#define EFL_CORE_TRAITS_FUNCTIONS_HPP

#include <type_traits>
#include <memory>
#include "Macros.hpp"

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
// TODO: Function traits
namespace H {
  template <typename T>
  auto Decl() NOEXCEPT
   -> MEflGTy(std::add_rvalue_reference<T>) {
      static_assert(sizeof(T) == 0, 
        "declval not allowed in an evaluated context");
  }

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

  namespace xx11 {
    template <typename T, typename = void>
    struct HasOverloadedAddress : FalseType { };

    template <typename T>
    struct HasOverloadedAddress<T,
      decltype(std::declval<T&>().operator&())>
     : TrueType { };

#    if CPPVER_LEAST(17)
    using ::std::addressof;
#    else
    template <typename T, 
      MEflEnableIf(!HasOverloadedAddress<T>::value)>
    NODISCARD FICONSTEXPR T* addressof(T& t) 
     NOEXCEPT { return &t; }

    template <typename T, 
      MEflEnableIf(HasOverloadedAddress<T>::value)>
    NODISCARD ALWAYS_INLINE T* addressof(T& t)
     NOEXCEPT { return std::addressof(t); }

    template <typename T>
    const T* addressof(const T&&) = delete;
#    endif

    template <typename T, typename...Args>
    NODISCARD ALWAYS_INLINE T* construct(T* t, Args&&...args) 
     NOEXCEPT(noexcept(T(std::forward<Args>(args)...))) {
      return ::new(static_cast<void*>(t)) 
        T(std::forward<Args>(args)...);
    }
  } // namespace xx11
} // namespace H
} // namespace C
} // namespace efl

#endif // EFL_CORE_TRAITS_FUNCTIONS_HPP
