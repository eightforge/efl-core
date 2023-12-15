//===- Panic/Internal.hpp -------------------------------------------===//
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

#pragma once

#ifndef EFLI_CORE_PANIC_INTERNAL_HPP
#define EFLI_CORE_PANIC_INTERNAL_HPP

#include <exception>
#include <Core/_Builtins.hpp>

#ifndef EFLI_PANICGUARD_
# define EFLI_PANICGUARD_ 0
#endif // Panic Guard

#ifndef EFLI_PANICSINGLE_
# define EFLI_PANICSINGLE_ 0
#endif

namespace std {
#if defined(EFLI_STL_LIBCPP_)

#elif defined (EFLI_STL_GLIBCXX_)


#endif // STL Check
} // namespace std

namespace efl {
namespace C {
/// Alias for `std::terminate_handler`.
using THandler = std::terminate_handler;
/// Alias for `std::exception_ptr`.
using ExPtr = std::exception_ptr;
} // namespace C
} // namespace efl

#endif // EFLI_CORE_PANIC_INTERNAL_HPP
