//===- Core/Fwd_/Option.hpp -----------------------------------------===//
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
//  Forward declarations for Option<...>.
//  Includes Option, SOption, NullOpt, and nullopt.
//
//===----------------------------------------------------------------===//

#ifndef EFL_CORE_FWD__OPTION_HPP
#define EFL_CORE_FWD__OPTION_HPP

#include <type_traits>
#include <CoreCommon/ConfigCache.hpp>

//=== Option Base ===//
namespace efl {
namespace C {
template <typename T>
struct Option;

namespace H {
  /// Decays `T` before passing to `Option<...>`.
  template <typename T>
  using decay_option_t = Option<
    typename std::decay<T>::type>;
} // namespace H
} // namespace C
} // namespace efl

//=== Null States ===//
#if CPPVER_LEAST(17)
# include <optional>
namespace efl::C {
/// Alias for std::nullopt_t.
using NullOpt = std::nullopt_t;
/// Alias for std::nullopt.
GLOBAL NullOpt nullopt = std::nullopt;
} // namespace efl::C
#else
namespace efl {
namespace C {
/// Type representing a null state.
struct NullOpt {
  enum class ENullOpt_ { enullopt_ };
  explicit constexpr NullOpt(ENullOpt_) NOEXCEPT { }
};
/// Instantiation of the `NullOpt` type.
GLOBAL NullOpt nullopt { NullOpt::ENullOpt_::enullopt_ };
} // namespace C
} // namespace efl
#endif // std::optional (C++17)

#endif // EFL_CORE_FWD__OPTION_HPP
