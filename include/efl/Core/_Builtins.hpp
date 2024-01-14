//===- Core/_Builtins.hpp -------------------------------------------===//
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
//  This file implements shared macros used internally.
//
//===----------------------------------------------------------------===//

#pragma once

#ifndef EFLH_CORE_BUILTINS_HPP
#define EFLH_CORE_BUILTINS_HPP

#include "_Builtins/CxprEval.hpp"
#include "_Builtins/Expect.hpp"
#include "_Builtins/FI128.hpp"
#include "_Builtins/Trap.hpp"
#include "_Builtins/Assert.hpp"

/// The namespace used for core. Useful as `*::core` is an alias.
#define BEGIN_EFL_CORE_NS namespace efl { namespace C {
#define END_EFL_CORE_NS }}

/// Gives the compiler a hint, may help with optimization.
#define EFL_ASSUME(...) EFLI_CORE_ASSUME_((__VA_ARGS__))
/// For use inside statements, assume true.
#define EFL_LIKELY(...) EFLI_EXPECT_TRUE_((__VA_ARGS__))
/// For use inside statements, assume false.
#define EFL_UNLIKELY(...) EFLI_EXPECT_FALSE_((__VA_ARGS__))
/// For a less aggressive assumption.
#define EFL_SOFT_LIKELY(...) EFLI_SOFT_TEXPECT_(bool, (__VA_ARGS__), true)
/// For a less aggressive assumption.
#define EFL_SOFT_UNLIKELY(...) EFLI_SOFT_TEXPECT_(bool, (__VA_ARGS__), false)
/// Marks code as unlikely to be executed.
#define EFL_COLD_PATH EFLI_COLD_PATH_
/// Unlikely at runtime. May help optimize.
#define EFL_RT_CXPREVAL() (EFL_UNLIKELY(EFLI_CXPREVAL_()))

/// Simple, `static_cast` based forwarding.
/// Prefer `std::forward` or `X11::cxpr_forward`.
#define FWD_CAST(e) static_cast<decltype(e)&&>(e)

/// Classic assert pseudofunction.
#define $raw_assert(expr) ( \
 (EFLI_EXPECT_TRUE_(!!(expr))) ? (void)(0) : \
  EFLI_UNDASSERT_(#expr) )

/// Assert with message.
#define $assert(expr, msg) ( \
 (EFLI_EXPECT_TRUE_(!!(expr))) ? (void)(0) : \
  EFLI_UNDASSERT_(msg) )

// Zig-style unreachable.
#if (COMPILER_DEBUG == 1)
/// Unreachable in constexpr contexts, otherwise a direct assert.
# define $unreachable ( \
 (EFLI_EXPECT_FALSE_(EFLI_CXPREVAL_())) ? \
  EFLI_UNREACHABLE_() : \
  EFLI_UNDASSERT_("reached unreachable code.") )
#else
/// Invokes the unreachable intrinsic.
# define $unreachable EFLI_UNREACHABLE_()
#endif

/// Creates a hard trap.
#define $trap() EFLI_TRAP_()

#if (COMPILER_DEBUG == 1)
/// Creates a debug trap.
# define $breakpoint() EFLI_DBGTRAP_()
#else
/// Noop in release.
# define $breakpoint()
#endif

#endif // EFLH_CORE_BUILTINS_HPP
