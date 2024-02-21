//===- Core/Box.hpp -------------------------------------------------===//
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
//  This file defines storage for a heap-allocated owning pointer wrapper.
//  Based on std::unique_ptr and Rust's Box.
//
//===----------------------------------------------------------------===//

#pragma once

#ifndef EFL_CORE_BOX_HPP
#define EFL_CORE_BOX_HPP

#include "StatelessAllocator.hpp"
// Handle.hpp -> PointerHandle<...>

namespace efl {
namespace C {
/**
 * 
 */
template <typename T, 
  H::SzType Align = alignof(T)>
struct BoxAllocator : StatelessMimAllocator<T, Align> {
  using Type = T;
  using Allocator = StatelessMimAllocator<T, Align>;
  using value_type = T;
  using pointer = typename Allocator::pointer;
public:
  /// Constructs object using Allocator::allocate(...).
  template <typename...Args>
  static pointer New(Args&&...args) {
    pointer data = Allocator::Allocate();
    return X11::construct(data, FWD_CAST(args)...);
  }

  /// Destroys object using Allocator::deallocate(...).
  static void Delete(pointer data) {
    if(EFL_SOFT_UNLIKELY(!data)) return;
    X11::destruct(data);
    Allocator::Deallocate(data);
  }
};

template <typename T,
  typename A = BoxAllocator<T>>
struct GSL_OWNER NODISCARD Box {
  MEflESAssert(!is_void<T>::value);
  using SelfType = Box<T, A>;
  using Type = T;
  using element_type = T;
  using pointer = element_type*;
  using allocator_type = A;
public:
  constexpr Box() = default;
  Box(const Box&) = delete;
  constexpr Box(std::nullptr_t) { }

  /// Move-constructs a `Box<T>`.
  Box(Box&& box) : data_(box.data_) {
    box.data_ = nullptr;
  }

  /// Move-assigns from a `Box<T>`.
  Box& operator=(Box&& box) {
    this->reset();
    this->data_ = box.data_;
    box.data_ = nullptr;
    return *this;
  }

  /// Calls `Allocator::Delete(...)`.
  ~Box() { if(data_) A::Delete(data_); }

private:
  HINT_INLINE explicit constexpr
   Box(T* data) : data_(data) { }

  Box& operator=(T* data) {
    this->reset();
    this->data_ = data;
    return *this;
  }

public:
  /// Allocates and constructs a new boxed object using `args...`.
  /// @return A new `Box<T, A>`.
  template <typename...Args>
  NODISCARD static SelfType New(Args&&...args) {
    T* p = A::New(FWD_CAST(args)...);
    return SelfType(p);
  }

  /// Allocates storage for an object using the provided allocator,
  /// then and constructs a new boxed object using `args...`.
  /// @return A new `Box<T, Allocator>`.
  template <typename Alloc, typename...Args>
  NODISCARD static Box<T, Alloc> 
   NewIn(Alloc alloc, Args&&...args) {
    MEflESAssert(H::IsStatelessAllocator<Alloc>::value);
    using PtrType = typename Alloc::pointer;
    PtrType p = alloc.New(FWD_CAST(args)...);
    return Box<T, Alloc>(p);
  }

  /// Constructs a boxed object from a `T*`.
  /// Warning: VERY DANGEROUS!!
  /// Only use when you know the correct allocator
  /// was used for a given pointer.
  /// @return A new `Box<T>`.
  NODISCARD static SelfType NewFromPtr(T* P) {
    return SelfType(P);
  }

  /// Explicitly destroys and deletes pointer.
  /// Useful for released values.
  static void Delete(T* ptr) {
    if(ptr) A::Delete(ptr);
  }

  //=== Modifiers ===//

  NODISCARD pointer release() NOEXCEPT {
    T* released_ptr = this->data_;
    this->data_ = nullptr;
    return released_ptr;
  }

  void reset() {
    if(EFL_SOFT_LIKELY(this->data_))
      A::Delete(this->data_);
    this->data_ = nullptr;
  }

  void swap(Box& box) NOEXCEPT {
    T* old_ptr = this->data_;
    this->data_ = box.data_;
    box.data_ = old_ptr;
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

} // namespace C
} // namespace efl

#endif // EFL_CORE_BOX_HPP
