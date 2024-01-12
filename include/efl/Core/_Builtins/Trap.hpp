//===- Core/_Builtins/Trap.hpp --------------------------------------===//
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

#pragma once

#ifndef EFLH_CORE_BUILTINS_TRAP_HPP
#define EFLH_CORE_BUILTINS_TRAP_HPP

#include "IConfig.hpp"

#if COMPILER_DEBUG
# include <cstdlib>
# define EFLI_QABORT_() ::abort()
#else
# define EFLI_QABORT_() EFLI_TRAP_()
#endif

#if __has_builtin(__builtin_trap) || defined(__GNUC__)
# define EFLI_TRAP_() __builtin_trap();
#elif defined(COMPILER_MSVC)
# define EFLI_TRAP_() __debugbreak()
#else
# define EFLI_TRAP_() \
  *reinterpret_cast<volatile int*>(0x11) = 0
#endif

#if __has_builtin(__builtin_debugtrap)
# define EFLI_DBGTRAP_() __builtin_debugtrap()
#elif defined(COMPILER_MSVC)
# define EFLI_DBGTRAP_() __debugbreak()
#else
# define EFLI_DBGTRAP_()
#endif

#endif // EFLH_CORE_BUILTINS_TRAP_HPP
