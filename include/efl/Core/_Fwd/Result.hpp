//===- Core/Fwd_/Result.hpp -----------------------------------------===//
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
//  Forward declarations for Result<...>.
//  Includes Result, Error, Unexpect, and unexpect.
//
//===----------------------------------------------------------------===//

#ifndef EFL_CORE_FWD__RESULT_HPP
#define EFL_CORE_FWD__RESULT_HPP

#include <CoreCommon/ConfigCache.hpp>

//=== Option Base ===//
namespace efl {
namespace C {
template <typename T, typename E>
struct Result;

template <typename E>
struct Error;

/// Used for the explicit construction of an error.
struct Unexpect {
  explicit Unexpect() = default;
};

/// Alias of `Unexpect` matching the stl.
using unexpect_t = Unexpect;

/// Allows for the explicit construction of an error.
GLOBAL Unexpect unexpect { };

} // namespace C
} // namespace efl

#endif // EFL_CORE_FWD__RESULT_HPP
