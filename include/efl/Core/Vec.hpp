//===- Core/Vec.hpp -------------------------------------------------===//
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
//  Provides a mimalloc-ed vector implementation.
//
//===----------------------------------------------------------------===//

#pragma once

#ifndef EFL_CORE_VEC_HPP
#define EFL_CORE_VEC_HPP

#include <vector>
#include "MimAllocator.hpp"

namespace efl {
namespace C {
/// `std::vector` alias utilizing mimalloc.
template <typename T,
  typename Allocator = MimAllocator<T>>
using Vec = std::vector<T, Allocator>;

/// Vector with an alignment larger than `alignof(T)`.
template <typename T, H::SzType Align>
using OveralignedVec = std::vector<
  T, MimAllocator<T, Align>>;

} // namespace C
} // namespace efl

#endif // EFL_CORE_VEC_HPP
