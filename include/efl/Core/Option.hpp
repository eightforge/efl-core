//===- Core/Option.hpp ----------------------------------------------===//
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
#include "Option/Cxx14Base.hpp"
#include "_Version.hpp"

// TODO: Add Option<T&> overloads

EFLI_CXPR11ASSERT_PROLOGUE_

namespace efl {
namespace C {
#if CPPVER_LEAST(17)
/// Wrapper around `std::optional<...>`.
template <typename T>
struct Option : std::optional<T> {
  using baseType_ = std::optional<T>;
  using std::optional<T>::operator=;
  using std::optional<T>::has_value;

  //=== Observers ===//

  FICONSTEXPR bool hasValue() CNOEXCEPT {
    return std::optional<T>::has_value();
  }

  /// Returns `true` if data is inactive.
  FICONSTEXPR bool isEmpty() CNOEXCEPT {
    return !std::optional<T>::has_value();
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

  //=== Monads ===//

  template <typename F>
  constexpr auto andThen(F&& f)& 
   -> remove_cvref_t<invoke_result_t<F, T&>> {
    if(*this) {
      return H::cxpr_forward<F>(f)(**this);
    } else {
      return remove_cvref_t<
        invoke_result_t<F, T&>>{ };
    }
  }

  template <typename F>
  constexpr auto andThen(F&& f)&&
   -> remove_cvref_t<invoke_result_t<F, T&&>> {
    if(*this) {
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
    if(*this) {
      return H::cxpr_forward<F>(f)(**this);
    } else {
      return remove_cvref_t<
        invoke_result_t<F, const T&>>{ };
    }
  }

  template <typename F>
  constexpr auto andThen(F&& f) const&&
   -> remove_cvref_t<invoke_result_t<F, const T&&>> {
    if(*this) {
      return H::cxpr_forward<F>(f)(
        H::cxpr_move(**this));
    } else {
      return remove_cvref_t<
        invoke_result_t<F, const T&&>>{ };
    }
  }

#if CPPVER_MOST(20)
  template <typename F>
  constexpr auto transform(F&& f)&
   -> Option<remove_cv_t<invoke_result_t<F, T&>>> {
    if(*this) {
      return { H::invoke(FWD_CAST(f)(**this)) };
    } else {
      return { nullopt };
    }
  }

  template <typename F>
  constexpr auto transform(F&& f)&&
   -> Option<remove_cv_t<invoke_result_t<F, T>>> {
    if(*this) {
      return { H::invoke(FWD_CAST(f)(
        H::cxpr_move(**this))) };
    } else {
      return { nullopt };
    }
  }

  template <typename F>
  constexpr auto transform(F&& f) const&
   -> Option<remove_cv_t<invoke_result_t<F, const T&>>> {
    if(*this) {
      return { H::invoke(FWD_CAST(f)(**this)) };
    } else {
      return { nullopt };
    }
  }

  template <typename F>
  constexpr auto transform(F&& f) const&& 
   -> Option<remove_cv_t<invoke_result_t<F, const T>>> {
    if(*this) {
      return { H::invoke(FWD_CAST(f)(
        H::cxpr_move(**this))) };
    } else {
      return { nullopt };
    }
  }
#else
  using std::optional<T>::transform;
#endif // C++23 Check (std::optional<...>::transform)

  template <typename F>
  constexpr Option orElse(F&& f)&& {
    static_assert(is_same_v<Option, invoke_result_t<F>>);
    return (*this) ? 
      H::cxpr_move(*this) : H::cxpr_forward<F>(f)();
  }

  template <typename F>
  constexpr Option orElse(F&& f) const& {
    static_assert(is_same_v<Option, invoke_result_t<F>>);
    return (*this) ? 
      *this : H::cxpr_forward<F>(f)();
  }
};

# ifdef __cpp_deduction_guides
template <typename T>
Option(T) -> Option<T>;
# endif // Deduction guides (C++17)
#else
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
    return X11::addressof(
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

  Option(const Option& O) : H::OptionBase<T>() {
    if(O.hasValue()) {
      (void) X11::construct(pdata(), *O);
      H::OptionBase<T>::active_ = true;
    }
  }

  Option(Option&& O) NOEXCEPT
   : H::OptionBase<T>() {
    if(O.hasValue()) {
      (void) X11::construct(pdata(), std::move(*O));
      H::OptionBase<T>::active_ = true;
    }
  }

  template <typename U, MEflEnableIf(
    is_constructible<T, const U&>::value)>
  Option(const Option<U>& O)
   : H::OptionBase<T>() {
    if(O.hasValue()) {
      (void) X11::construct(pdata(), *O);
      H::OptionBase<T>::active_ = true;
    }
  }

  template <typename U, MEflEnableIf(
    is_constructible<T, U&&>::value)>
  Option(Option<U>&& O) NOEXCEPT
   : H::OptionBase<T>() {
    if(O.hasValue()) {
      (void) X11::construct(
        pdata(), std::move(*O));
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

  Option& operator=(const Option& O) {
    if(O.active()) {
      if(this->active()) {
        this->unwrap() = *O;
      } else this->initialize(*O);
    } else {
      this->clear();
    }
    return *this;
  }

  Option& operator=(Option&& O) NOEXCEPT {
    if(O.active()) {
      if(this->active()) {
        this->unwrap() = std::move(*O);
      } else this->initialize(std::move(*O));
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

  void swap(Option& O) {
    if(this->active() && O.active()) {
      using std::swap;
      swap(**this, *O);
    } else if(this->active()) {
      O.emplace(std::move(**this));
      this->clear();
    } else if(O.active()) {
      this->emplace(std::move(*O));
      O.clear();
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

  /// Returns `true` if data is inactive.
  FICONSTEXPR bool isEmpty() CNOEXCEPT {
    return !this->has_value();
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
    EFLI_CXPR11ASSERT_(this->active());
    return H::OptionBase<T>::data_.data_;
  }

  constexpr const T& unwrap() const&& {
    EFLI_CXPR11ASSERT_(this->active());
    return H::cxpr_move(
      H::OptionBase<T>::data_.data_);
  }

  EFLI_CXX14_CXPR_ type_* operator->() {
    EFLI_DBGASSERT_(this->active());
    return this->pdata();
  }

  FICONSTEXPR const T* operator->() const {
    EFLI_CXPR11ASSERT_(this->active());
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
    EFLI_CXPR11ASSERT_(this->active());
    return this->unwrap();
  }

  constexpr const T&& operator*() const&& {
    EFLI_CXPR11ASSERT_(this->active());
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
      return H::cxpr_move(this->unwrap());
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
  EFLI_CXX14_CXPR_ auto transform(F&& f)&
   -> Option<remove_cv_t<invoke_result_t<F, T&>>> {
    if(this->active()) {
      return { H::invoke(FWD_CAST(f)(**this)) };
    } else {
      return { nullopt };
    }
  }

  template <typename F>
  EFLI_CXX14_CXPR_ auto transform(F&& f)&&
   -> Option<remove_cv_t<invoke_result_t<F, T>>> {
    if(this->active()) {
      return { H::invoke(FWD_CAST(f)(
        H::cxpr_move(**this))) };
    } else {
      return { nullopt };
    }
  }

  template <typename F>
  constexpr auto transform(F&& f) const&
   -> Option<remove_cv_t<invoke_result_t<F, const T&>>> {
    if(this->active()) {
      return { H::invoke(FWD_CAST(f)(**this)) };
    } else {
      return { nullopt };
    }
  }

  template <typename F>
  constexpr auto transform(F&& f) const&& 
   -> Option<remove_cv_t<invoke_result_t<F, const T>>> {
    if(this->active()) {
      return { H::invoke(FWD_CAST(f)(
        H::cxpr_move(**this))) };
    } else {
      return { nullopt };
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
#endif

template <typename T>
struct Option<T&> {
private:
  FICONSTEXPR bool active() CNOEXCEPT { return !!data_; }
  void clear() NOEXCEPT { this->data_ = nullptr; }
public:
  using value_type = T;
  constexpr Option() NOEXCEPT : data_(nullptr) { }
  constexpr Option(NullOpt) NOEXCEPT : Option() { }
  constexpr Option(T& t) : data_(X11::addressof(t)) { }
  Option(const Option& O) : data_(O.data_) { }

  Option(Option&& O) NOEXCEPT : data_(O.data_) {
    O.data_ = nullptr;
  }

  template <typename U, MEflEnableIf(
    is_base_of<T, U>::value)>
  Option(U& u) : Option((T&)u) { }

  template <typename U, MEflEnableIf(
    is_base_of<T, U>::value)>
  Option(const Option<U&>& O)
   : data_((T*) O.data_) { }
  
  template <typename U, MEflEnableIf(
    is_base_of<T, U>::value)>
  Option(const Option<U&>&& O)
   : data_((T*) O.data_) {
    O.data_ = nullptr;
  }

  EFLI_CXX20_CXPR_ ~Option() = default;

  Option& operator=(NullOpt) NOEXCEPT {
    this->clear();
    return *this;
  }

  Option& operator=(const Option& O) {
    this->data_ = O.data_;
    return *this;
  }

  Option& operator=(Option&& O) NOEXCEPT {
    this->data_ = O.data_;
    O.data_ = nullptr;
    return *this;
  }

  template <typename U, MEflEnableIf(
    is_base_of<T, U>::value)>
  Option& operator=(U& u) {
    this->data_ = X11::addressof((T&)u);
    return *this;
  }

  //=== Modifiers ===//

  template <typename U>
  void emplace(U& u) { *this = u; }

  ALWAYS_INLINE void reset() NOEXCEPT { 
    this->clear();
  }

  void swap(Option& O) {
    T* tmp = O.data_;
    O.data_ = this->data_;
    this->data_ = tmp;
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
    return this->active();
  }

  /// Returns `true` if data is not active.
  FICONSTEXPR bool isEmpty() CNOEXCEPT {
    return !this->active();
  }

  EFLI_CXX14_CXPR_ T& unwrap() {
    EFLI_CXPRASSERT_(this->active());
    return *data_;
  }

  constexpr const T& unwrap() const {
    EFLI_CXPR11ASSERT_(this->active());
    return *data_;
  }

  EFLI_CXX14_CXPR_ T* operator->() {
    EFLI_DBGASSERT_(this->active());
    return data_;
  }

  FICONSTEXPR const T* operator->() const {
    EFLI_CXPR11ASSERT_(this->active());
    return data_;
  }

  EFLI_CXX14_CXPR_ T& operator*() {
    EFLI_DBGASSERT_(this->active());
    return this->unwrap();
  }

  constexpr const T& operator*() const {
    EFLI_CXPR11ASSERT_(this->active());
    return this->unwrap();
  }

  template <typename U>
  EFLI_CXX14_CXPR_ T& unwrapOr(U& u) {
    if(this->active()) {
      return this->unwrap();
    } else {
      return static_cast<T&>(u);
    }
  }

  template <typename U>
  constexpr const T& unwrapOr(const U& u) const {
    if(this->active()) {
      return H::cxpr_move(this->unwrap());
    } else {
      return static_cast<const T&>(u);
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
  EFLI_CXX14_CXPR_ auto transform(F&& f)&
   -> Option<remove_cv_t<invoke_result_t<F, T&>>> {
    if(this->active()) {
      return { H::invoke(FWD_CAST(f)(**this)) };
    } else {
      return { nullopt };
    }
  }

  template <typename F>
  EFLI_CXX14_CXPR_ auto transform(F&& f)&&
   -> Option<remove_cv_t<invoke_result_t<F, T>>> {
    if(this->active()) {
      return { H::invoke(FWD_CAST(f)(
        H::cxpr_move(**this))) };
    } else {
      return { nullopt };
    }
  }

  template <typename F>
  constexpr auto transform(F&& f) const&
   -> Option<remove_cv_t<invoke_result_t<F, const T&>>> {
    if(this->active()) {
      return { H::invoke(FWD_CAST(f)(**this)) };
    } else {
      return { nullopt };
    }
  }

  template <typename F>
  constexpr auto transform(F&& f) const&& 
   -> Option<remove_cv_t<invoke_result_t<F, const T>>> {
    if(this->active()) {
      return { H::invoke(FWD_CAST(f)(
        H::cxpr_move(**this))) };
    } else {
      return { nullopt };
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

private:
  T* data_ = nullptr;
};

} // namespace C
} // namespace efl

#include "Option/Compare.hpp"
#include "Option/Helpers.hpp"

EFLI_CXPR11ASSERT_EPILOGUE_

#endif // EFL_CORE_OPTION_HPP
