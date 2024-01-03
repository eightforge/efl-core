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

#include <memory>
#include <efl/Core/_Version.hpp>
#include "Std.hpp"

#define EFLI_RMREF_(...) typename \
  ::std::remove_reference<__VA_ARGS__>::type

#if __has_builtin(__builtin_addressof)
# define EFLI_ADDRESSOF_(x) __builtin_addressof(x)
# define EFLI_ADDRESSOF_ATTRIB_ NODISCARD FICONSTEXPR
#else
# define EFLI_ADDRESSOF_(x) std::addressof(x)
# define EFLI_ADDRESSOF_ATTRIB_ NODISCARD ALWAYS_INLINE
#endif

namespace efl {
namespace C {
// TODO: Function traits
namespace H {
  template <typename T>
  auto Decl() NOEXCEPT
   -> add_rvalue_reference_t<T> {
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

#if CPPVER_LEAST(17)
    using ::std::addressof;
#else
    template <typename T, 
      MEflEnableIf(!HasOverloadedAddress<T>::value)>
    NODISCARD FICONSTEXPR T* addressof(T& t) 
     NOEXCEPT { return &t; }

    template <typename T, 
      MEflEnableIf(HasOverloadedAddress<T>::value)>
    EFLI_ADDRESSOF_ATTRIB_ T* addressof(T& t)
     NOEXCEPT { return EFLI_ADDRESSOF_(t); }

    template <typename T>
    const T* addressof(const T&&) = delete;
#endif // constexpr std::addressof check (C++17)

    template <typename T, typename...Args>
    NODISCARD FICONSTEXPR auto construct(T* t, Args&&...args) 
     noexcept(noexcept(T(cxpr_forward<Args>(args)...)))
     -> decltype(::new((void*)0) T(Decl<Args>()...)) {
      return ::new(static_cast<void*>(t)) 
        T(cxpr_forward<Args>(args)...);
    }

    template <typename T, MEflEnableIf(
      is_trivially_destructible<T>::value)>
    EFLI_CXX14_CXPR_ void destruct(T*) { }

    template <typename T, MEflEnableIf(
      (!is_trivially_destructible<T>::value))>
    EFLI_CXX14_CXPR_ void destruct(T* t) {
      static_assert(!is_void<T>::value, 
        "You cannot pass a void* to destruct.");
      (!!t) ? void(t->~T()) : void(0);
    }
  } // namespace xx11
} // namespace H
} // namespace C
} // namespace efl

#undef EFLI_ADDRESSOF_
#undef EFLI_ADDRESSOF_ATTRIB_
#undef EFLI_RMREF_

#endif // EFL_CORE_TRAITS_FUNCTIONS_HPP
