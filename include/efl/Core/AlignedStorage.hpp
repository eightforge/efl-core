//===- Core/AlignedStorage.hpp --------------------------------------===//
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
//  This file defines storage for types like variant and optional.
//
//===----------------------------------------------------------------===//

#pragma once

#ifndef EFL_CORE_ALIGNEDSTORAGE_HPP
#define EFL_CORE_ALIGNEDSTORAGE_HPP

#include "Fundamental.hpp"

namespace efl {
namespace C {
/// Basic `std::aligned_storage`-esque container.
template <H::SzType Bytes, H::SzType Align>
struct BasicAlignedStorage {
  alignas(Align) ubyte data[Bytes];
};

/// Aligned storage for a specific type.
template <typename T>
using AlignedStorage = 
  BasicAlignedStorage<sizeof(T), alignof(T)>;

/// Aligned "union" for a set of types.
template <typename T, typename...TT>
struct AlignedUnion {
  using baseType_ = std::aligned_union<0, T, TT...>;
  alignas(baseType_) ubyte data[sizeof(baseType_)]; 
};
} // namespace C
} // namespace efl

#endif // EFL_CORE_ALIGNEDSTORAGE_HPP
