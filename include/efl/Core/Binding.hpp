//===- Core/Binding.hpp ---------------------------------------------===//
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
//  This file defines deferrable reference wrappers.
//
//===----------------------------------------------------------------===//

#pragma once

#ifndef EFL_CORE_BINDING_HPP
#define EFL_CORE_BINDING_HPP

#include "Box.hpp"
#include "Fundamental.hpp"
#include "Traits/Functions.hpp"
#include "_Cxx11Assert.hpp"
#include "_Version.hpp"

EFLI_CXPR11ASSERT_PROLOGUE_

namespace efl {
namespace C {
/**
 * Non-owning wrapper around a single object.
 * Acts as a deferrable reference.
 */
template <typename T>
struct GSL_POINTER Binding {
  using Type = T;
  using SelfType = Binding<T>;
  using value_type = T;
  using pointer = T*;
  using reference = T&;
  using size_type = H::SzType;
public:
  /// Default constructed.
  constexpr Binding() = default;

  /// Construct directly.
  EFLI_CXX17_CXPR_ Binding(T& t) 
   : data_(X11::addressof(t)) { }
  
  /// Construct from a `Box<T>`. Must be initialized.
  EFLI_CXX20_CXPR_ Binding(Box<T>& box) : data_(box.get()) {
    $assert(!box.isEmpty(), "Box cannot be empty.");
  }

  /// Construct from a `const Box<T>`. Must be initialized.
  EFLI_CXX20_CXPR_ Binding(const Box<T>& box) : data_(box.get()) {
    $assert(!box.isEmpty(), "Box cannot be empty.");
  }

  /// Construct from another `Binding<...>`.
  constexpr Binding(const Binding& binding) 
   : data_(binding.get()) {
    EFLI_CXPR11ASSERT_(!binding.isEmpty());
  }

  EFLI_CXX20_CXPR_ Binding& operator=(T& t) {
    this->data_ = X11::addressof(t);
    return *this;
  }

  EFLI_CXX20_CXPR_ Binding& operator=(Binding binding) {
    $assert(!binding.isEmpty(), "Must be bound to assign.");
    this->data_ = binding.data_;
    return *this;
  }

  //=== Observers ===//

  ALWAYS_INLINE bool isEmpty() const NOEXCEPT
  { return !this->data_; }

  explicit operator bool() const NOEXCEPT 
  { return bool(this->data_); }

  pointer get() const NOEXCEPT 
  { return this->data_; }

  T* operator->() const NOEXCEPT {
    $raw_assert(!this->isEmpty());
    return this->data_;
  }

  Type& operator*() NOEXCEPT {
    $raw_assert(!this->isEmpty());
    return *this->data_;
  }

  const Type& operator*() const NOEXCEPT {
    $raw_assert(!this->isEmpty());
    return *this->data_;
  }

private:
  T* data_ = nullptr;
};

//=== Deduction Guides ===//

template <typename T>
EFLI_CXX17_CXPR_ Binding<T>
 make_binding(T& t) NOEXCEPT {
  return Binding<T>(t);
}

template <typename T>
NODEBUG Binding<T>
 make_binding(Box<T>& box) NOEXCEPT {
  return Binding<T>(box);
}

template <typename T>
NODEBUG Binding<T>
 make_binding(const Box<T>& box) NOEXCEPT {
  return Binding<T>(box);
}

template <typename T>
NODEBUG Binding<T>&
 make_binding(Binding<T>& binding) NOEXCEPT {
  return binding;
}

#ifdef __cpp_deduction_guides
template <typename T>
Binding(T&) -> Binding<T>;

template <typename T>
Binding(Box<T>&) -> Binding<T>;

template <typename T>
Binding(const Box<T>&) -> Binding<T>;
#endif

} // namespace C
} // namespace efl

EFLI_CXPR11ASSERT_EPILOGUE_

#endif // EFL_CORE_BINDING_HPP
