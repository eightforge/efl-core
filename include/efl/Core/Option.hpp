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
//
//===----------------------------------------------------------------===//

#pragma once

#ifndef EFL_CORE_OPTION_HPP
#define EFL_CORE_OPTION_HPP

#include "AlignedStorage.hpp"

#if CPPVER_LEAST(17)
# include "Traits.hpp"
# include <optional>
#else
# include "Option/Cxx14Base.hpp"
#endif

#ifndef EFLI_OPASSERT_
# if NDEBUG
#  define EFLI_OPASSERT_(...)
# else
#  define EFLI_OPASSERT_(...) assert(__VA_ARGS__)
# endif
#endif

#if EFLI_MUTABLE_CXPR_ == 1
# define EFLI_OPMUTCXPR_ constexpr
#else
# define EFLI_OPMUTCXPR_
#endif

#if CPPVER_LEAST(17)
namespace efl::C {
  /// Alias for std::nullopt_t.
  using NullOpt = std::nullopt_t;
  /// Alias for std::nullopt.
  GLOBAL NullOpt nullopt = std::nullopt;

  // TODO: Option<...> interface
} // namespace efl::C
#else
namespace efl {
namespace C {
  using H::NullOpt;
  using H::nullopt;

  template <typename T>
  struct Option;

  /// Decays `T` before passing to `Option<...>`.
  template <typename T>
  using SOption = Option<MEflGTy(std::decay<T>)>;

  template <typename T>
  struct Option : private H::OptionBase<T> {
  private:
    using type_ = typename H::OptionBase<T>::type_;
    using typeDec_ = MEflGTy(std::decay<T>);
    static_assert(!std::is_same<typeDec_, NullOpt>::value, 
      "Cannot create an optional NullOpt.");
    static_assert(!std::is_same<typeDec_, in_place_t>::value, 
      "Cannot create an optional in_place_t.");

    FICONSTEXPR bool active() CNOEXCEPT {
      return H::OptionBase<T>::active_;
    }

    ALWAYS_INLINE type_* pdata() {
      return std::addressof(
        H::OptionBase<T>::data_.data_);
    }

    FICONSTEXPR const T* pdata() CONST {
      return H::xx11::addressof(
        H::OptionBase<T>::data_.data_);
    }

    void clear() NOEXCEPT {
      H::OptionBase<T>::clear();
    }

    template <typename...Args>
    void initialize(Args&&...args) 
     NOEXCEPT(noexcept(type_(FWD(args)...))) {
      EFLI_OPASSERT_(!this->active());
      (void) H::xx11::construct(
        pdata(), FWD(args)...);
      H::OptionBase<T>::active_ = true;
    }
  
  public:
    using value_type = T;

    constexpr Option() NOEXCEPT : H::OptionBase<T>() { }
    constexpr Option(NullOpt) NOEXCEPT : H::OptionBase<T>() { }

    template <typename U = T, MEflEnableIf(
      std::is_constructible<T, U&&>::value &&
      !std::is_same<MEflGTy(std::decay<U>), Option<T>>::value &&
      !std::is_same<MEflGTy(std::decay<U>), in_place_t>::value)>
    constexpr Option(U&& u) 
     : H::OptionBase<T>(EFLI_CXPRFWD_(u)) { }

    Option(const Option& op) : H::OptionBase<T>() {
      if(op.hasValue()) {
        (void) H::xx11::construct(pdata(), *op);
        H::OptionBase<T>::active_ = true;
      }
    }

    Option(Option&& op) NOEXCEPT(
     std::is_nothrow_move_constructible<T>::value)
     : H::OptionBase<T>() {
      if(op.hasValue()) {
        (void) H::xx11::construct(pdata(), std::move(*op));
        H::OptionBase<T>::active_ = true;
      }
    }

    template <typename U, MEflEnableIf(
      std::is_constructible<T, const U&>::value)>
    Option(const Option<T>& op)
     : H::OptionBase<T>() {
      if(op.hasValue()) {
        (void) H::xx11::construct(pdata(), *op);
        H::OptionBase<T>::active_ = true;
      }
    }

    template <typename U, MEflEnableIf(
      std::is_constructible<T, U&&>::value)>
    Option(Option<T>&& op) NOEXCEPT(
     std::is_nothrow_constructible<T, U&&>::value)
     : H::OptionBase<T>() {
      if(op.hasValue()) {
        (void) H::xx11::construct(
          pdata(), std::move(*op));
        H::OptionBase<T>::active_ = true;
      }
    }

    template <typename...TT>
    explicit constexpr Option(in_place_t ip, TT&&...tt)
     : H::OptionBase<T>(ip, EFLI_CXPRFWD_(tt)...) { }
    
    template <typename U, typename...TT>
    explicit constexpr Option(H::InitList<U> ls, TT&&...tt)
     : H::OptionBase<T>(ls, EFLI_CXPRFWD_(tt)...) { } 

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

    Option& operator=(Option&& op) NOEXCEPT(
     std::is_nothrow_move_constructible<T>::value &&
     std::is_nothrow_move_assignable<T>::value) {
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
      std::is_constructible<type_, U>::value)>
    Option& operator=(U&& u) {
      if(this->active()) {
        this->unwrap() = FWD(u);
      } else this->initialize(FWD(u));
      return *this;
    }

    template <typename...Args>
    ALWAYS_INLINE void emplace(Args&&...args) {
      this->clear();
      this->initialize(FWD(args)...);
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

    FICONSTEXPR const T& unwrap() CONST& {
      return H::OptionBase<T>::data_.data_;
    }

    ALWAYS_INLINE EFLI_OPMUTCXPR_ type_& unwrap()& {
      return H::OptionBase<T>::data_.data_;
    }

    EFLI_OPMUTCXPR_ type_&& unwrap()&& {
      return EFLI_CXPRMV_(
        H::OptionBase<T>::data_.data_);
    }

    EFLI_OPMUTCXPR_ type_* operator->() {
      EFLI_OPASSERT_(!this->active());
      return this->pdata();
    }

    FICONSTEXPR const T* operator->() CONST {
      return this->pdata();
    }

    EFLI_OPMUTCXPR_ type_& operator*()& {
      EFLI_OPASSERT_(!this->active());
      return this->unwrap();
    }

    EFLI_OPMUTCXPR_ type_&& operator*()&& {
      EFLI_OPASSERT_(!this->active());
      return EFLI_CXPRMV_(this->unwrap());
    }

    constexpr const T& operator*() CONST& {
      return this->unwrap();
    }

    constexpr const T&& operator*() CONST&& {
      return EFLI_CXPRMV_(this->unwrap());
    }

    template <typename U>
    EFLI_OPMUTCXPR_ T unwrapOr(U&& u) CONST& {
      if(this->active()) {
        return this->unwrap();
      } else {
        return static_cast<T>(EFLI_CXPRFWD_(u));
      }
    }

    template <typename U>
    EFLI_OPMUTCXPR_ T unwrapOr(U&& u)&& {
      if(this->active()) {
        return EFLI_CXPRMV_(this->unwrap());
      } else {
        return static_cast<T>(EFLI_CXPRFWD_(u));
      }
    }

    //=== Monads (Not fully supported) ===//

    template <typename F>
    EFLI_OPMUTCXPR_ auto andThen(F&& f)& 
     -> remove_cvref_t<invoke_result_t<F, T&>> {
      if(this->active()) {
        return EFLI_CXPRFWD_(f)(**this);
      } else {
        return remove_cvref_t<
          invoke_result_t<F, T&>>{ };
      }
    }

    template <typename F>
    EFLI_OPMUTCXPR_ auto andThen(F&& f)&&
     -> remove_cvref_t<invoke_result_t<F, T&&>> {
      if(this->active()) {
        return EFLI_CXPRFWD_(f)(
          EFLI_CXPRMV_(**this));
      } else {
        return remove_cvref_t<
          invoke_result_t<F, T&&>>{ };
      }
    }

    template <typename F>
    EFLI_OPMUTCXPR_ auto andThen(F&& f) CONST& 
     -> remove_cvref_t<invoke_result_t<F, const T&>> {
      if(this->active()) {
        return EFLI_CXPRFWD_(f)(**this);
      } else {
        return remove_cvref_t<
          invoke_result_t<F, const T&>>{ };
      }
    }

    template <typename F>
    EFLI_OPMUTCXPR_ auto andThen(F&& f) CONST&&
     -> remove_cvref_t<invoke_result_t<F, const T&&>> {
      if(this->active()) {
        return EFLI_CXPRFWD_(f)(
          EFLI_CXPRMV_(**this));
      } else {
        return remove_cvref_t<
          invoke_result_t<F, const T&&>>{ };
      }
    }

    void reset() NOEXCEPT { this->clear(); }
  };


} // namespace C
} // namespace efl
#endif

#undef EFLI_OPASSERT_
#undef EFLI_OPMUTCXPR_

#endif // EFL_CORE_OPTION_HPP
