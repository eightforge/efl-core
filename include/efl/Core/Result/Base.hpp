//===- Core/Result/Base.hpp -----------------------------------------===//
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

#pragma once

#ifndef EFL_CORE_RESULT_BASE_HPP
#define EFL_CORE_RESULT_BASE_HPP

#include <cassert>
#include <efl/Core/Traits.hpp>
#include <efl/Core/_Fwd/Result.hpp>
#include <efl/Core/_Cxx11Assert.hpp>
#include <efl/Core/_Version.hpp>

namespace efl {
namespace C {
namespace H {
namespace result_ {
  template <typename T, typename E>
  struct IsTrivial : BoolC<
    is_trivially_destructible<E>::value &&
    (is_trivially_destructible<T>::value || 
    (is_void<T>::value || is_same<T, Dummy>::value))> { };
  
  template <typename>
  struct IsResult : FalseType { };

  template <typename T, typename E>
  struct IsResult<Result<T, E>> : TrueType { };

  template <typename>
  struct IsError : FalseType { };

  template <typename E>
  struct IsError<Error<E>> : TrueType { };

  template <typename T>
  struct IsReservedTag : BoolC<
    is_same<remove_cvref_t<T>, in_place_t>::value ||
    is_same<remove_cvref_t<T>, unexpect_t>::value> { };

  template <typename E>
  struct IsValidError : BoolC<
    is_object<E>::value && (!is_array<E>::value) &&
    (!is_const<E>::value) && (!is_volatile<E>::value) &&
    (!IsError<E>::value) && (!IsReservedTag<E>::value)> { };
  
  template <typename T>
  using ReifyVoid = conditional_t<
    (!is_void<T>::value), T, Dummy>;
} // namespace result_

/// Storage for trivial objects.
template <typename T, typename E, bool = 
  result_::IsTrivial<T, E>::value>
union ResultStorage {
  constexpr ResultStorage() NOEXCEPT : data_() { }

  template <typename...TT>
  constexpr ResultStorage(in_place_t, TT&&...tt)
   : data_(cxpr_forward<TT>(tt)...) { }
  
  template <typename U, typename...TT>
  constexpr ResultStorage(InitList<U> il, TT&&...tt)
   : data_(il, cxpr_forward<TT>(tt)...) { }

  template <typename...TT>
  constexpr ResultStorage(Unexpect, TT&&...tt)
   : err_(cxpr_forward<TT>(tt)...) { }
  
  template <typename U, typename...TT>
  constexpr ResultStorage(Unexpect, InitList<U> il, TT&&...tt)
   : err_(il, cxpr_forward<TT>(tt)...) { }

public:
  T data_;
  E err_;
};

/// Storage for non-trivial objects.
template <typename T, typename E>
union ResultStorage<T, E, false> {
  template <typename U = T, 
    MEflEnableIf(is_default_constructible<T>::value)>
  constexpr ResultStorage() NOEXCEPT : data_() { }

  template <typename...TT>
  constexpr ResultStorage(in_place_t, TT&&...tt)
   : data_(cxpr_forward<TT>(tt)...) { }
  
  template <typename U, typename...TT>
  constexpr ResultStorage(InitList<U> il, TT&&...tt)
   : data_(il, cxpr_forward<TT>(tt)...) { }
  
  template <typename...TT>
  constexpr ResultStorage(Unexpect, TT&&...tt)
   : err_(cxpr_forward<TT>(tt)...) { }
  
  template <typename U, typename...TT>
  constexpr ResultStorage(Unexpect, InitList<U> il, TT&&...tt)
   : err_(il, cxpr_forward<TT>(tt)...) { }

  EFLI_CXX20_CXPR_ ~ResultStorage() { }

public:
  T data_;
  E err_;
};

/// Base for trivial results.
template <typename T, typename E, bool = 
  result_::IsTrivial<T, E>::value>
struct ResultBase {
  using type_ = result_::ReifyVoid<T>;
public:
  constexpr ResultBase() = default;

  constexpr ResultBase(const T& t)
   : data_(in_place, t), active_(true) { }
  
  constexpr ResultBase(T&& t)
   : data_(in_place, cxpr_move(t)), active_(true) { }
  
  template <typename...TT>
  constexpr ResultBase(in_place_t ip, TT&&...tt)
   : data_(ip, cxpr_forward<TT>(tt)...), active_(true) { }
  
  template <typename U, typename...TT>
  constexpr ResultBase(InitList<U> il, TT&&...tt)
   : data_(in_place, il, cxpr_forward<TT>(tt)...), active_(true) { }
  
  template <typename...TT>
  constexpr ResultBase(Unexpect ux, TT&&...tt)
   : data_(ux, cxpr_forward<TT>(tt)...), active_(false) { }
  
  template <typename U, typename...TT>
  constexpr ResultBase(Unexpect ux, InitList<U> il, TT&&...tt)
   : data_(ux, il, cxpr_forward<TT>(tt)...), active_(false) { }

private:
  ALWAYS_INLINE EFLI_CXX14_CXPR_ 
   void destroy() NOEXCEPT { }

public:
  ResultStorage<type_, E> data_;
  bool active_ = is_default_constructible<type_>::value;
};

/// Base for non-trivial results.
template <typename T, typename E>
struct ResultBase<T, E, false> {
  using type_ = result_::ReifyVoid<T>;
public:
  constexpr ResultBase() = default;

  constexpr ResultBase(const T& t)
   : data_(in_place, t), active_(true) { }
  
  constexpr ResultBase(T&& t)
   : data_(in_place, cxpr_move(t)), active_(true) { }
  
  template <typename...TT>
  constexpr ResultBase(in_place_t ip, TT&&...tt)
   : data_(ip, cxpr_forward<TT>(tt)...), active_(true) { }
  
  template <typename U, typename...TT>
  constexpr ResultBase(InitList<U> il, TT&&...tt)
   : data_(in_place, il, cxpr_forward<TT>(tt)...), active_(true) { }
  
  template <typename...TT>
  constexpr ResultBase(Unexpect ux, TT&&...tt)
   : data_(ux, cxpr_forward<TT>(tt)...), active_(false) { }
  
  template <typename U, typename...TT>
  constexpr ResultBase(Unexpect ux, InitList<U> il, TT&&...tt)
   : data_(ux, il, cxpr_forward<TT>(tt)...), active_(false) { }

  EFLI_CXX17_CXPR_ ~ResultBase() {
    this->destroy();
  }

protected:
  ALWAYS_INLINE EFLI_CXX17_CXPR_ 
   void destroy() NOEXCEPT {
    if(active_) xx11::destruct(&data_.data_);
    else xx11::destruct(&data_.err_);
  }

public:
  ResultStorage<type_, E> data_;
  bool active_ = is_default_constructible<type_>::value;
};

} // namespace H
} // namespace C
} // namespace efl

#endif // EFL_CORE_RESULT_BASE_HPP
