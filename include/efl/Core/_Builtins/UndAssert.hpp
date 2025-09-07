//===- Core/_Builtins/UndAssert.hpp ---------------------------------===//
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
//  Implements the underlying assertion functions on different platforms.
//  See https://gist.github.com/9inefold/a14edbfb83e140c939b71e2939e4a237.
//
//===----------------------------------------------------------------===//

#pragma once

#ifndef EFLH_CORE_BUILTINS_UNDASSERT_HPP
#define EFLH_CORE_BUILTINS_UNDASSERT_HPP

#include "IConfig.hpp"

#define CSTR_ const char*
#define UINT_ unsigned int

EFLI_BEGIN_CSTD_
#if defined(PLATFORM_WINDOWS)
# define EFLI_UNDASSERT_(msg) \
 ::_assert((msg), __FILE__, (unsigned)(__LINE__))
  void __cdecl _assert(CSTR_ msg, CSTR_ file, UINT_ line);
#elif defined(PLATFORM_APPLE)
# if __DARWIN_UNIX03
#  define EFLI_UNDASSERT_(msg) \
  ::__assert_rtn(COMPILER_FUNCTION, __FILE__, (unsigned)(__LINE__), (msg))
   NORETURN void __assert_rtn(
    CSTR_ func, CSTR_ file, UINT_ line, CSTR_ msg);
# else
#  define EFLI_UNDASSERT_(msg) \
  ::__assert((msg), __FILE__, (unsigned)(__LINE__))
   NORETURN void __assert(CSTR_ msg, CSTR_ file, UINT_ line);
# endif // __DARWIN_UNIX03?
#elif defined(PLATFORM_ANDROID)
# define EFLI_UNDASSERT_(msg) \
 ::__assert2(__FILE__, __LINE__, COMPILER_FUNCTION, (msg))
 NONNULL() void __assert2(
  CSTR_ file, int line, CSTR_ func, CSTR_ msg) __noreturn;
#elif defined(PLATFORM_SOLARIS)
# define EFLI_UNDASSERT_(msg) ::__assert_c99((msg), __FILE__, \
  (unsigned)(__LINE__), COMPILER_FUNCTION)
 NORETURN __assert_c99(
  CSTR_ msg, CSTR_ file, UINT_ line, CSTR_ func);
#elif defined(__AVR__) // Arduino
# define EFLI_UNDASSERT_(...) ::abort()
 void abort(void) GNU_NORETURN;
#else
# define EFLI_UNDASSERT_(msg) \
 ::__assert_fail((msg), __FILE__, (unsigned)(__LINE__), COMPILER_FUNCTION)
 NORETURN void __assert_fail(
  CSTR_ msg, CSTR_ file, UINT_ line, CSTR_ func) noexcept(true);
#endif // Platform Check
EFLI_END_CSTD_

#undef CSTR_
#undef UINT_

#endif // EFLH_CORE_BUILTINS_UNDASSERT_HPP
