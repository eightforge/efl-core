//===- Core/Traits/Invoke.hpp ---------------------------------------===//
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
//  This file implements the invoke sections of <functional>.
//
//===----------------------------------------------------------------===//

#pragma once

#ifndef EFL_CORE_TRAITS_INVOKE_HPP
#define EFL_CORE_TRAITS_INVOKE_HPP

#include "Functions.hpp"
#include "StdInt.hpp"

/*
 * These are useful when using constexpr functions,
 * since it allows `invoke` to work correctly in C++11.
 */
#if CPPVER_MOST(11)
# define CXX11Void \
  ::efl::C::xx11::WrapResult<void>
# define CXX11Return() return { }
#else
# define CXX11Void void
# define CXX11Return() do {} while(0)
#endif // C++11 Check

namespace efl {
namespace C {
namespace H {
template <typename Void, 
  typename F, typename...>
struct IsInvokable : FalseType { };

template <typename F, typename...Args>
struct IsInvokable<decltype(void(std::declval<F>()(
 std::declval<Args>()...))), F, Args...> : TrueType { };
} // namespace H

template <typename F, typename...Args>
using is_invokable = H::IsInvokable<void, F, Args...>;

#if CPPVER_LEAST(14)
template <typename F, typename...Args>
GLOBAL bool is_invokable_v = is_invokable<F, Args...>::value;
#endif

//=== invoke(...) ===//
namespace H {
namespace H1 {
// Not strictly necessary, but looks better...
# define EFLI_MPINV_(b, mp, args) \
  ((b).*mp)(EFLI_CXPRFWD_(args)...)

  // Used when the first argument can be upcast to `Base`.
  // Essentially just "normal" invoations.
  template <class Base, typename U,
    bool = std::is_base_of<Base, 
      MEflGTy(std::decay<U>)>::value>
  struct MPInvokeHelper {
    template <typename Ret, typename...Args>
    constexpr auto operator()(
     Ret Base::* mp, U u, Args&&...args) CONST NOEXCEPT(
     noexcept(EFLI_MPINV_(EFLI_CXPRFWD_(u), mp, args)))
     -> decltype(EFLI_MPINV_(std::declval<U>(), mp, args)) {
      return EFLI_MPINV_(EFLI_CXPRFWD_(u), mp, args);
    }

    // For the special case of `Ret` not being invokable.
    // Not sure why this isn't a compilation error, but
    // I'd rather be consistent with the STL.
    template <typename Ret, typename...Args>
    constexpr MEflGTy(std::enable_if<
      !is_invokable<Ret, Args...>::value, Ret>)
     operator()(Ret Base::* mp, U u, Args&&...args) CNOEXCEPT {
      return ((*EFLI_CXPRFWD_(u)).*mp);
    }
  };

  // Used when the first argument cannot be upcast.
  // The assumption is that the argument is a wrapper,
  // and a valid type can be accessed with `operator*`.
  template <class Base, typename U>
  struct MPInvokeHelper<Base, U, false> {
    template <typename Ret, typename...Args>
    constexpr auto operator()(
     Ret Base::* mp, U u, Args&&...args) CONST NOEXCEPT(
     noexcept(EFLI_MPINV_((*EFLI_CXPRFWD_(u)), mp, args)))
     -> decltype(EFLI_MPINV_((*std::declval<U>()), mp, args)) {
      return EFLI_MPINV_((*EFLI_CXPRFWD_(u)), mp, args);
    }

    // Same as the default instantiation.
    template <typename Ret, typename...Args>
    constexpr MEflGTy(std::enable_if<
      !is_invokable<Ret, Args...>::value, Ret>)
     operator()(Ret Base::* mp, U u, Args&&...args) CNOEXCEPT {
      return ((*EFLI_CXPRFWD_(u)).*mp);
    }
  };

#  undef EFLI_MPINV_

  //=== Direct Invoke Helper ===//

  // Normal dispatch.
  template <typename F, 
    bool = !std::is_member_pointer<
      MEflGTy(std::decay<F>)>::value>
  struct InvokeHelper {
    template <typename...Args>
    constexpr auto operator()(F f, Args&&...args) CONST NOEXCEPT(
     noexcept(EFLI_CXPRFWD_(f)(EFLI_CXPRFWD_(args)...)))
     -> decltype(std::declval<F>()(std::declval<Args>()...)) {
      return EFLI_CXPRFWD_(f)(EFLI_CXPRFWD_(args)...);
    }
  };

  // Member pointer specialization.
  // Dispatch code is so ugly
  template <typename MP>
  struct InvokeHelper<MP, false> {
    template <typename Ret, class Base, typename U, typename...Args>
    constexpr auto operator()(
     Ret Base::* mp, U&& u, Args&&...args) CONST NOEXCEPT(
      noexcept(MPInvokeHelper<Base, U>{}(mp, 
       EFLI_CXPRFWD_(u), EFLI_CXPRFWD_(args)...)))
     -> decltype(MPInvokeHelper<Base, U>{}(mp, 
       EFLI_CXPRFWD_(u), EFLI_CXPRFWD_(args)...)) {
      return MPInvokeHelper<Base, U>{}(mp, 
        EFLI_CXPRFWD_(u), EFLI_CXPRFWD_(args)...);
    }
  };
} // namespace H1

/**
 * @brief Invokes a callable object with the given arguments.
 * 
 * This function does way more than you would expect.
 * If `f` is directly invokable (eg. you can do `f(args...)`),
 * it stops there, but if `f` is a member pointer, we have to
 * figure out if the first argument is derived from the type
 * it's bound to, and if the type it returns on access is
 * callable or not.
 */
template <typename F, typename...Args>
FICONSTEXPR auto invoke(F&& f, Args&&...args) NOEXCEPT(
 noexcept(H1::InvokeHelper<F>{}(
   EFLI_CXPRFWD_(f), cxpr_forward<Args>(args)...)))
 -> decltype(H1::InvokeHelper<F>{}(
   EFLI_CXPRFWD_(f), cxpr_forward<Args>(args)...)) {
  return H1::InvokeHelper<F>{}(
   // Use `cxpr_forward` directly here, 
   // otherwise intellisense starts complaining.
   EFLI_CXPRFWD_(f), cxpr_forward<Args>(args)...); 
}
} // namespace H

namespace xx11 {
  // Used in C++11 for `constexpr` functions.
  template <typename T>
  struct WrapResult 
  { using type = T; };
} // namespace xx11

template <typename F, typename...Args>
using invoke_result_t = decltype(
  H::invoke(std::declval<F>(), std::declval<Args>()...));
} // namespace C
} // namespace efl

#endif // EFL_CORE_TRAITS_INVOKE_HPP
