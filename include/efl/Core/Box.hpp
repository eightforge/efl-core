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

#include "MimAllocator.hpp"
#include "Traits/Std.hpp"
#include "_Builtins.hpp"
// Casts.hpp -> dyn_cast
// Handle.hpp -> PointerHandle<...>

namespace efl {
namespace C {
// AllocBase <- is_empty<A>
template <typename T,
  H::SzType Align = alignof(T)>
struct MSVC_EMPTY_BASES BoxAllocator 
 : protected MimAllocator<T, Align> {
  using Type = T;
  using Allocator = MimAllocator<T, Align>;
  using pointer = T*;
public:

};

template <typename T,
  typename A = MimAllocator<T>>
struct GSL_OWNER NODISCARD Box {
  
};

} // namespace C
} // namespace efl

#endif // EFL_CORE_BOX_HPP
