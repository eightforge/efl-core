//===- Core/Fwd_/Hash.hpp -------------------------------------------===//
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
//  Forward declarations for Hash<...>.
//
//===----------------------------------------------------------------===//

#ifndef EFL_CORE_FWD__HASH_HPP
#define EFL_CORE_FWD__HASH_HPP

#include <CoreCommon/ConfigCache.hpp>

// TODO: Implement wyhash

namespace efl {
namespace C {
/// Faster hash implementation.
template <typename T, typename = void>
struct Hash {
  COMPILE_FAILURE(T, 
    "Hash has not been implemented for this type.")
};

} // namespace C
} // namespace efl

#endif // EFL_CORE_FWD__HASH_HPP