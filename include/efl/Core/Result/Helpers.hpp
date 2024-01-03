//===- Core/Result/Base.hpp -----------------------------------------===//
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
//  This file implements the helper classes/functions for Option<...>.
//  It includes the macros `$Ok` and `$Err`.
//
//===----------------------------------------------------------------===//

#pragma once

#ifndef EFL_CORE_RESULT_HELPERS_HPP
#define EFL_CORE_RESULT_HELPERS_HPP

#include "Base.hpp"

#define $Ok(...) { ::efl::C::in_place, __VA_ARGS__ }
#define $Err(...) { ::efl::C::unexpect, __VA_ARGS__ }

namespace efl {
namespace C {

} // namespace C
} // namespace efl

#endif // EFL_CORE_RESULT_HELPERS_HPP
