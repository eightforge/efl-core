//===- Core/StatelessAllocator.hpp ----------------------------------===//
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
//  This file defines an allocator which forces static functions
//  for allocating storage.
//
//===----------------------------------------------------------------===//

#pragma once

#ifndef EFL_CORE_STATELESSALLOCATOR_HPP
#define EFL_CORE_STATELESSALLOCATOR_HPP

#include "MimAllocator.hpp"
#include "Traits/Functions.hpp"
#include "_Builtins.hpp"

namespace efl {
namespace C {
namespace H {
  template <typename A, 
    typename HasStaticAllocate = void, 
    typename HasStaticDeallocate = void>
  struct IsStatelessAllocator 
   : H::FalseType { };
  
  template <typename A>
  struct IsStatelessAllocator<A,
    void_t<decltype(A::allocate(0UL))>,
    void_t<decltype(A::deallocate(
      (typename A::value_type*)(nullptr), 0UL))>>
   : H::BoolC<is_empty<A>::value> { };
} // namespace H

template <typename A, 
  bool = H::IsStatelessAllocator<A>::value>
struct StatelessAllocator {
  COMPILE_FAILURE(A, "Allocator must provide a static "
    "allocate/deallocate function, and be empty.");
};

template <typename A>
struct StatelessAllocator<A, true> {
  using Type = typename A::value_type;
  using Allocator = A;
  using value_type = Type;
  using pointer = typename A::pointer;
public:
  /// Invokes the allocator's `allocate` function.
  static pointer Allocate(H::SzType n = 1) {
    $assert(n, "Cannot allocate 0 objects.");
    return Allocator::allocate(n);
  }

  /// Invokes the allocator's `deallocate` function.
  static void Deallocate(pointer data, H::SzType n = 1) {
    if(EFL_UNLIKELY(!data || n == 0)) return;
    Allocator::deallocate(data, n);
  }
};

template <typename T, 
  H::SzType Align = alignof(T)>
using StatelessMimAllocator =
  StatelessAllocator<MimAllocator<T, Align>>;

} // namespace C
} // namespace efl

#endif // EFL_CORE_STATELESSALLOCATOR_HPP
