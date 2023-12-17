//===- Core/Option.hpp ----------------------------------------------===//
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
//  This file either implements a pre C++17 optional, or expands on
//  the current std::optional API. Based on the N3793 implementation.
//  Be warned: exceptions are not supported. Use unwrapping macros
//  where possible.
//
//===----------------------------------------------------------------===//

#pragma once

#ifndef EFL_CORE_OPTION_HPP
#define EFL_CORE_OPTION_HPP

#include "_Fwd/Option.hpp"
#include "AlignedStorage.hpp"
#include "Option/Cxx14Base.hpp"
#include "_Builtins.hpp"
#include "_Version.hpp"

LLVM_IGNORED("-Wc++14-extensions")
GNU_IGNORED("-Wc++14-extensions")

#if CPPVER_LEAST(14) || (defined(COMPILER_GCC) || defined(COMPILER_CLANG))
# define EFLI_OPCXPRASSERT_(...) \
  EFLI_DBGASSERT_(__VA_ARGS__)
#else
# define EFLI_OPCXPRASSERT_(...) (void)(0)
#endif

#if CPPVER_LEAST(17)
namespace efl::C {
/// Wrapper around `std::optional<...>`.
template <typename T>
struct Option : std::optional<T> {
  using baseType_ = std::optional<T>;
  using std::optional<T>::operator=;

  //=== Observers ===//

  FICONSTEXPR bool hasValue() CNOEXCEPT {
    return std::optional<T>::has_value();
  }

  FICONSTEXPR auto unwrap() NOEXCEPT {
    EFLI_DBGASSERT_(this->hasValue());
    return std::optional<T>::value();
  }

  FICONSTEXPR auto unwrap() CNOEXCEPT {
    EFLI_DBGASSERT_(this->hasValue());
    return std::optional<T>::value();
  }

  template <typename U>
  FICONSTEXPR T unwrapOr(U&& u)&& {
    return std::optional<T>::value_or(FWD(u));
  }

  template <typename U>
  FICONSTEXPR T unwrapOr(U&& u) const& {
    return std::optional<T>::value_or(FWD(u));
  }
};

#ifdef __cpp_deduction_guides
template <typename T>
Option(T) -> Option<T>;
#endif // Deduction guides (C++17)
} // namespace efl::C
#else
namespace efl {
namespace C {
/// C++11/14 optional implementation.
template <typename T>
struct Option : private H::OptionBase<T> {
private:
  using type_ = typename H::OptionBase<T>::type_;
  using typeDec_ = decay_t<T>;
  static_assert(is_different<typeDec_, NullOpt>::value, 
    "Cannot create an optional NullOpt.");
  static_assert(is_different<typeDec_, in_place_t>::value, 
    "Cannot create an optional in_place_t.");

  FICONSTEXPR bool active() CNOEXCEPT {
    return H::OptionBase<T>::active_;
  }

  ALWAYS_INLINE type_* pdata() {
    return std::addressof(
      H::OptionBase<T>::data_.data_);
  }

  FICONSTEXPR const T* pdata() const {
    return H::xx11::addressof(
      H::OptionBase<T>::data_.data_);
  }

  void clear() NOEXCEPT {
    H::OptionBase<T>::clear();
  }

  template <typename...Args>
  void initialize(Args&&...args) 
   noexcept(noexcept(type_(FWD(args)...))) {
    EFLI_DBGASSERT_(!this->active());
    (void) X11::construct(
      pdata(), FWD(args)...);
    H::OptionBase<T>::active_ = true;
  }

public:
  using value_type = T;
  constexpr Option() NOEXCEPT : H::OptionBase<T>() { }
  constexpr Option(NullOpt) NOEXCEPT : H::OptionBase<T>() { }

  template <typename U = T, MEflEnableIf(
   conjunction<
    is_constructible<T, U&&>,
    is_different<decay_t<T>, Option<T>>,
    is_different<decay_t<T>, in_place_t>
   >::value)>
  constexpr Option(U&& u) 
   : H::OptionBase<T>(H::cxpr_forward<U>(u)) { }

  Option(const Option& op) : H::OptionBase<T>() {
    if(op.hasValue()) {
      (void) X11::construct(pdata(), *op);
      H::OptionBase<T>::active_ = true;
    }
  }

  Option(Option&& op) noexcept(
   is_nothrow_move_constructible<T>::value)
   : H::OptionBase<T>() {
    if(op.hasValue()) {
      (void) X11::construct(pdata(), std::move(*op));
      H::OptionBase<T>::active_ = true;
    }
  }

  template <typename U, MEflEnableIf(
    is_constructible<T, const U&>::value)>
  Option(const Option<T>& op)
   : H::OptionBase<T>() {
    if(op.hasValue()) {
      (void) X11::construct(pdata(), *op);
      H::OptionBase<T>::active_ = true;
    }
  }

  template <typename U, MEflEnableIf(
    is_constructible<T, U&&>::value)>
  Option(Option<T>&& op) noexcept(
   is_nothrow_constructible<T, U&&>::value)
   : H::OptionBase<T>() {
    if(op.hasValue()) {
      (void) X11::construct(
        pdata(), std::move(*op));
      H::OptionBase<T>::active_ = true;
    }
  }

  template <typename...TT>
  explicit constexpr Option(in_place_t ip, TT&&...tt)
   : H::OptionBase<T>(ip, H::cxpr_forward<TT>(tt)...) { }
  
  template <typename U, typename...TT>
  explicit constexpr Option(
    in_place_t ip, H::InitList<U> ls, TT&&...tt)
   : H::OptionBase<T>(ls, H::cxpr_forward<TT>(tt)...) { } 

  ~Option() = default;

  Option& operator=(NullOpt) NOEXCEPT {
    this->clear();
    return *this;
  }

  Option& operator=(const Option& op) {
    if(op.active()) {
      if(this->active()) {
        this->unwrap() = *op;
      } else this->initialize(*op);
    } else {
      this->clear();
    }
    return *this;
  }

  Option& operator=(Option&& op) noexcept(
   is_nothrow_move_constructible<T>::value &&
   is_nothrow_move_assignable<T>::value) {
    if(op.active()) {
      if(this->active()) {
        this->unwrap() = std::move(*op);
      } else this->initialize(std::move(*op));
    } else {
      this->clear();
    }
    return *this;
  }

  template <typename U, MEflEnableIf(
    is_constructible<type_, U>::value)>
  Option& operator=(U&& u) {
    if(this->active()) {
      this->unwrap() = FWD(u);
    } else this->initialize(FWD(u));
    return *this;
  }

  //=== Modifiers ===//

  template <typename...Args>
  void emplace(Args&&...args) {
    this->clear();
    this->initialize(FWD(args)...);
  }

  ALWAYS_INLINE void reset() NOEXCEPT { 
    this->clear();
  }

  void swap(Option& op) {
    if(this->active() && op.active()) {
      using std::swap;
      swap(**this, *op);
    } else if(this->active()) {
      op.emplace(std::move(**this));
      this->clear();
    } else if(op.active()) {
      this->emplace(std::move(*op));
      op.clear();
    }
  }

  //=== Observers ===//

  /// Returns `true` if data is active.
  FICONSTEXPR explicit operator bool() CNOEXCEPT {
    return this->active();
  }

  /// STL naming, returns `true` if data is active.
  constexpr bool has_value() CNOEXCEPT {
    return this->active();
  }

  /// Returns `true` if data is active.
  FICONSTEXPR bool hasValue() CNOEXCEPT {
    return this->has_value();
  }

  EFLI_CXX14_CXPR_ type_& unwrap()& {
    EFLI_CXPRASSERT_(this->active());
    return H::OptionBase<T>::data_.data_;
  }

  EFLI_CXX14_CXPR_ type_&& unwrap()&& {
    EFLI_CXPRASSERT_(this->active());
    return H::cxpr_move(
      H::OptionBase<T>::data_.data_);
  }

  constexpr const T& unwrap() const& {
    EFLI_OPCXPRASSERT_(this->active());
    return H::OptionBase<T>::data_.data_;
  }

  constexpr const T& unwrap() const&& {
    EFLI_OPCXPRASSERT_(this->active());
    return H::cxpr_move(
      H::OptionBase<T>::data_.data_);
  }

  EFLI_CXX14_CXPR_ type_* operator->() {
    EFLI_DBGASSERT_(this->active());
    return this->pdata();
  }

  FICONSTEXPR const T* operator->() const {
    EFLI_OPCXPRASSERT_(this->active());
    return this->pdata();
  }

  EFLI_CXX14_CXPR_ type_& operator*()& {
    EFLI_DBGASSERT_(this->active());
    return this->unwrap();
  }

  EFLI_CXX14_CXPR_ type_&& operator*()&& {
    EFLI_DBGASSERT_(this->active());
    return H::cxpr_move(this->unwrap());
  }

  constexpr const T& operator*() const& {
    EFLI_OPCXPRASSERT_(this->active());
    return this->unwrap();
  }

  constexpr const T&& operator*() const&& {
    EFLI_OPCXPRASSERT_(this->active());
    return H::cxpr_move(this->unwrap());
  }

  template <typename U>
  constexpr T unwrapOr(U&& u) const& {
    if(this->active()) {
      return this->unwrap();
    } else {
      return static_cast<T>(
        H::cxpr_forward<U>(u));
    }
  }

  template <typename U>
  EFLI_CXX14_CXPR_ T unwrapOr(U&& u)&& {
    if(this->active()) {
      return EFLI_CXPRMV_(this->unwrap());
    } else {
      return static_cast<T>(
        H::cxpr_forward<U>(u));
    }
  }

  //=== Monads (Not fully supported) ===//

  template <typename F>
  EFLI_CXX14_CXPR_ auto andThen(F&& f)& 
   -> remove_cvref_t<invoke_result_t<F, T&>> {
    if(this->active()) {
      return H::cxpr_forward<F>(f)(**this);
    } else {
      return remove_cvref_t<
        invoke_result_t<F, T&>>{ };
    }
  }

  template <typename F>
  EFLI_CXX14_CXPR_ auto andThen(F&& f)&&
   -> remove_cvref_t<invoke_result_t<F, T&&>> {
    if(this->active()) {
      return H::cxpr_forward<F>(f)(
        H::cxpr_move(**this));
    } else {
      return remove_cvref_t<
        invoke_result_t<F, T&&>>{ };
    }
  }

  template <typename F>
  constexpr auto andThen(F&& f) const& 
   -> remove_cvref_t<invoke_result_t<F, const T&>> {
    if(this->active()) {
      return H::cxpr_forward<F>(f)(**this);
    } else {
      return remove_cvref_t<
        invoke_result_t<F, const T&>>{ };
    }
  }

  template <typename F>
  constexpr auto andThen(F&& f) const&&
   -> remove_cvref_t<invoke_result_t<F, const T&&>> {
    if(this->active()) {
      return H::cxpr_forward<F>(f)(
        H::cxpr_move(**this));
    } else {
      return remove_cvref_t<
        invoke_result_t<F, const T&&>>{ };
    }
  }

  template <typename F>
  EFLI_CXX14_CXPR_ Option orElse(F&& f)&& {
    MEflESAssert(is_same<Option, 
      invoke_result_t<F>>::value);
    return (this->active()) ? 
      H::cxpr_move(*this) : 
      H::cxpr_forward<F>(f)();
  }

  template <typename F>
  constexpr Option orElse(F&& f) const& {
    MEflESAssert(is_same<Option, 
      invoke_result_t<F>>::value);
    return (this->active()) ? 
      *this : H::cxpr_forward<F>(f)();
  }
};
} // namespace C
} // namespace efl
#endif

#undef EFLI_OPCXPRASSERT_
#undef EFLI_CXX14_CXPR_

#include "Option/Compare.hpp"
#include "Option/Helpers.hpp"

GNU_POP()
LLVM_POP()

#endif // EFL_CORE_OPTION_HPP
