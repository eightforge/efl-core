//===- Core/Preload.hpp ---------------------------------------------===//
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
//  This file defines an object that can be used for simple and safe
//  static initialization. It should essentially never be used in
//  header files.
//
//===----------------------------------------------------------------===//

#pragma once

#ifndef EFL_CORE_PRELOAD_HPP
#define EFL_CORE_PRELOAD_HPP

#include "Fundamental.hpp"
#include "Traits/Functions.hpp"
#include "Traits/Invoke.hpp"

namespace efl {
namespace C {
namespace H {
  /// Default constructs an object.
  template <typename T>
  struct DefaultInitialize {
    constexpr T operator()() const NOEXCEPT 
    { return T(); }
  };

  /// Does nothing.
  template <>
  struct DefaultInitialize<void> {
    constexpr CXX11Void operator()() const NOEXCEPT 
    { CXX11Return(); }
  };
} // namespace H

/**
 * @brief Safely constructs an object.
 * @tparam T The type of the object to be created.
 * @tparam Init A functor which creates a `T`.
 */
template <typename T, 
  typename Init = H::DefaultInitialize<T>>
struct Preload {
  using Type = T;
  using Ref = T&;
  using CRef = const T&;
public:
  constexpr Preload() 
   : data_(Preload::Create()) { }
  
  Ref get()& { return this->data_; }
  CRef get() const& { return this->data_; }

  Ref operator()()& { return this->data_; }
  CRef operator()() const& { return this->data_; }

  Type* operator->()& 
  { return X11::addressof(data_); }

  const Type* operator->() const& 
  { return X11::addressof(data_); }

private:
  FICONSTEXPR static Type Create() 
   NOEXCEPT(noexcept(Init{}())) 
  { return Init{}(); }

private:
  Type data_;
};

/**
 * @brief `void` specialization. Just runs a function.
 * @tparam Init A functor which runs once.
 */
template <typename Init>
struct Preload<void, Init> {
  using Type = void;
  Preload() { (void)(Init{}()); }
};

/// Creates a `Preload<void, F>`.
template <typename F>
using StaticExec = Preload<void, F>;

} // namespace C
} // namespace efl

#endif // EFL_CORE_PRELOAD_HPP
