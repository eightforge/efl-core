//===- Panic/Guard.cpp ----------------------------------------------===//
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
//  This file provides the guards for panic handling (if required).
//
//===----------------------------------------------------------------===//

#pragma once

#ifndef EFLI_CORE_PANIC_GUARD_HPP
#define EFLI_CORE_PANIC_GUARD_HPP

#include "Internal.hpp"

#if EFLI_PANICGUARD_ == 1
# include <Core/Mtx.hpp>
/// Creates a lock in a multithreaded environment.
# define EFLI_PANIC_LOCK_(...) \
  MEflLock(::efl::C::panicSync())

namespace efl {
namespace C {
namespace {
  inline Mtx& panicSync() {
    static Mtx mtx { };
    return mtx;
  }
} // namespace `anonymous`
} // namespace C
} // namespace efl
#else
/// Does nothing.
# define EFLI_PANIC_LOCK_(...) (void)(0)
#endif // Guards enabled?

#endif // EFLI_CORE_PANIC_GUARD_HPP
