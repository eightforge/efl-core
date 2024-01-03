//===- Core/Result.hpp ----------------------------------------------===//
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
//  This file implements the Result type, which is used to
//  represent a type that can contain a value or an error.
//  Allows for cleaner and more readable code.
//
//===----------------------------------------------------------------===//

#pragma once

#ifndef EFL_CORE_RESULT_HPP
#define EFL_CORE_RESULT_HPP

#include "_Fwd/Result.hpp"
#include "Result/Base.hpp"
#include "_Version.hpp"

EFLI_CXPR11ASSERT_PROLOGUE_

// TODO: Finish implementation

namespace efl {
namespace C {
/// @brief An object representing an error state.
/// @tparam E Type of the error.
template <typename E>
struct Error {
  static_assert(!is_void<E>::value, "Errors cannot be void!");
  MEflESAssert(H::result_::IsValidError<E>::value);
public:
  constexpr Error() = default;
  constexpr Error(const Error&) = default;
  constexpr Error(Error&&) = default;

  template <typename Err = E, MEflEnableIf(
    (!H::result_::IsReservedTag<Err>::value) &&
    is_constructible<E, Err>::value)>
  constexpr Error(Err&& e) : err_(e) { }

  template <typename...TT>
  constexpr Error(in_place_t, TT&&...tt)
   : err_(H::cxpr_forward<TT>(tt)...) { }
  
  template <typename U, typename...TT>
  constexpr Error(in_place_t, H::InitList<U> il, TT&&...tt)
   : err_(il, H::cxpr_forward<TT>(tt)...) { }

  //=== Modifiers/Observers ===//

  EFLI_CXX14_CXPR_ void swap(Error& err)
   noexcept(is_nothrow_swappable<E>::value) {
    MEflESAssert(is_swappable<E>::value);
    using std::swap;
    swap(this->err_, err.err_);
  }

  NODISCARD ALWAYS_INLINE EFLI_CXX14_CXPR_ E& error()& 
   NOEXCEPT { return err_; }

  NODISCARD EFLI_CXX14_CXPR_ E&& error()&& 
   NOEXCEPT { return H::cxpr_move(err_); }

  NODISCARD FICONSTEXPR const E& error() const& 
   NOEXCEPT { return err_; }

  NODISCARD constexpr const E&& error() const&&
   NOEXCEPT { return H::cxpr_move(err_); }

  template <typename E2>
  NODISCARD friend EFLI_CXX14_CXPR_ bool
   operator==(const Error& lhs, const Error<E2>& rhs) {
    return lhs.err_ == rhs.error();
  }

private:
  E err_;
};

/// @brief An object which can contain a value or an error.
/// @tparam T The type of the value state.
/// @tparam E The type of the error state.
template <typename T, typename E>
struct Result : private H::ResultBase<T, E> {
private:
  static_assert(!is_reference<T>::value);
  static_assert(!is_function<T>::value);
  static_assert(!H::result_::IsReservedTag<T>::value);
  static_assert(!H::result_::IsError<remove_cv_t<T>>::value );
  static_assert(H::result_::IsValidError<E>::value);
  using type_ = H::ResultBase<T, E>::type_;
public:
  using value_type = T;
  using error_type = E;
  using unexpected_type = Error<E>;

  template <typename U>
  using rebind = Result<U, E>;

  FICONSTEXPR bool has_value() const NOEXCEPT {
    return H::ResultBase<T, E>::active_;
  }

private:
  ALWAYS_INLINE EFLI_CXX14_CXPR_ void 
   clear() NOEXCEPT {
    H::ResultBase<T, E>::destroy();
  }

  ALWAYS_INLINE EFLI_CXX14_CXPR_ type_* pdata() {
    return std::addressof(
      H::ResultBase<T, E>::data_.data_);
  }

  FICONSTEXPR const type_* pdata() const {
    return H::xx11::addressof(
      H::ResultBase<T, E>::data_.data_);
  }

  ALWAYS_INLINE EFLI_CXX14_CXPR_ E* perr() {
    return std::addressof(
      H::ResultBase<T, E>::data_.err_);
  }

  FICONSTEXPR const E* perr() const {
    return H::xx11::addressof(
      H::ResultBase<T, E>::data_.err_);
  }

  template <typename...Args>
  void initialize_data(Args&&...args) 
   noexcept(noexcept(type_(FWD(args)...))) {
    this->clear();
    (void) X11::construct(
      pdata(), FWD(args)...);
    H::ResultBase<T, E>::active_ = true;
  }

  template <typename...Args>
  void initialize_err(Args&&...args) 
   noexcept(noexcept(E(FWD(args)...))) {
    this->clear();
    (void) X11::construct(
      perr(), FWD(args)...);
    H::ResultBase<T, E>::active_ = false;
  }

public:
  constexpr Result() = default;

  constexpr Result(const Result& res)
   noexcept(conjunction<
     is_nothrow_copy_constructible<type_>,
     is_nothrow_copy_constructible<E>>::value)
   : H::ResultBase<T, E>::active_(res.active_) {
    if(res.has_value())
      X11::construct(pdata(), res.data_.data_);
    else
      X11::construct(perr(), res.data_.err_);
  }

  constexpr Result(Result&& res)
   noexcept(conjunction<
     is_nothrow_move_constructible<type_>,
     is_nothrow_move_constructible<E>>::value)
   : H::ResultBase<T, E>::active_(res.active_) {
    if(res.has_value())
      X11::construct(pdata(),
        H::cxpr_move(res).data_.data_);
    else
      X11::construct(perr(),
        H::cxpr_move(res).data_.err_);
  }

  template <typename U = T, MEflEnableIf(
    is_convertible<U, T>::value)>
  constexpr Result(U&& u)
   noexcept(is_nothrow_constructible<T, U>::value)
   : H::ResultBase<T, E>(
    in_place, H::cxpr_forward<U>(u)) { }

  template <typename Err = E, MEflEnableIf(
    is_convertible<const Err&, E>::value)>
  constexpr Result(const Error<Err>& err)
   noexcept(is_nothrow_copy_constructible<Err>::value)
   : H::ResultBase<T, E>(unexpect, err.error()) { }
  
  template <typename Err = E, MEflEnableIf(
    is_convertible<Err&&, E>::value)>
  constexpr Result(Error<Err>&& err)
   noexcept(is_nothrow_move_constructible<Err>::value)
   : H::ResultBase<T, E>(
    unexpect, H::cxpr_move(err).error()) { }
  
  template <typename...Args>
  constexpr Result(in_place_t ip, Args&&...args)
   : H::ResultBase<T, E>(
    ip, H::cxpr_forward<Args>(args)...) { }
  
  template <typename...Args>
  constexpr Result(unexpect_t ux, Args&&...args)
   : H::ResultBase<T, E>(
    ux, H::cxpr_forward<Args>(args)...) { }

  EFLI_CXX20_CXPR_ ~Result() = default;

  EFLI_CXX17_CXPR_ Result&
   operator=(const Result& res) {
    if(res.has_value()) initialize_data(res.unwrap());
    else initialize_err(res.error());
    return *this;
  }

  EFLI_CXX17_CXPR_ Result&
   operator=(Result&& res) {
    if(res.has_value()) initialize_data(
      H::cxpr_move(res).unwrap());
    else initialize_err(
      H::cxpr_move(res).error());
    return *this;
  }

  //=== Observers ===//

  /// Returns `true` if data is active.
  FICONSTEXPR explicit operator bool() CNOEXCEPT {
    return this->has_value();
  }

  /// Returns `true` if data is active.
  FICONSTEXPR bool hasValue() const NOEXCEPT {
    return this->has_value();
  }

  EFLI_CXX14_CXPR_ T& unwrap()& {
    EFLI_CXPRASSERT_(this->has_value());
    return H::ResultBase<T, E>::data_.data_;
  }

  EFLI_CXX14_CXPR_ T&& unwrap()&& {
    EFLI_CXPRASSERT_(this->has_value());
    return H::cxpr_move(
      H::ResultBase<T, E>::data_.data_);
  }

  constexpr const T& unwrap() const& {
    EFLI_CXPR11ASSERT_(this->has_value());
    return H::ResultBase<T, E>::data_.data_;
  }

  constexpr const T& unwrap() const&& {
    EFLI_CXPR11ASSERT_(this->has_value());
    return H::cxpr_move(
      H::ResultBase<T, E>::data_.data_);
  }

  EFLI_CXX14_CXPR_ type_* operator->() {
    EFLI_DBGASSERT_(this->has_value());
    return this->pdata();
  }

  FICONSTEXPR const T* operator->() const {
    EFLI_CXPR11ASSERT_(this->has_value());
    return this->pdata();
  }

  EFLI_CXX14_CXPR_ type_& operator*()& {
    EFLI_DBGASSERT_(this->has_value());
    return this->unwrap();
  }

  EFLI_CXX14_CXPR_ type_&& operator*()&& {
    EFLI_DBGASSERT_(this->has_value());
    return H::cxpr_move(this->unwrap());
  }

  constexpr const T& operator*() const& {
    EFLI_CXPR11ASSERT_(this->has_value());
    return this->unwrap();
  }

  constexpr const T&& operator*() const&& {
    EFLI_CXPR11ASSERT_(this->has_value());
    return H::cxpr_move(this->unwrap());
  }

  EFLI_CXX14_CXPR_ E& error()& {
    EFLI_CXPRASSERT_(!this->has_value());
    return H::ResultBase<T, E>::data_.err_;
  }

  EFLI_CXX14_CXPR_ E&& error()&& {
    EFLI_CXPRASSERT_(!this->has_value());
    return H::cxpr_move(
      H::ResultBase<T, E>::data_.err_);
  }

  constexpr const E& error() const& {
    EFLI_CXPR11ASSERT_(!this->has_value());
    return H::ResultBase<T, E>::data_.err_;
  }

  constexpr const E& error() const&& {
    EFLI_CXPR11ASSERT_(!this->has_value());
    return H::cxpr_move(
      H::ResultBase<T, E>::data_.err_);
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

  //=== Modifiers ===//

  template <typename...Args>
  void emplace(Args&&...args) {
    this->initialize_data(FWD(args)...);
  }
};

#ifdef __cpp_deduction_guides
template <typename T>
Error(T) -> Error<T>;
#endif // Deduction guides (C++17)
} // namespace C
} // namespace efl

#include "Result/Helpers.hpp"

EFLI_CXPR11ASSERT_EPILOGUE_

#endif // EFL_CORE_RESULT_HPP
