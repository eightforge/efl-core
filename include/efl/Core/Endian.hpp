//===- Core/Endian.hpp ----------------------------------------------===//
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

#ifndef EFL_CORE_ENDIAN_HPP
#define EFL_CORE_ENDIAN_HPP

#include "_Builtins.hpp"

#if defined(__cpp_lib_endian) || CPPVER_LEAST(20)
# include <bit>
# define EFLI_ENDIAN_STL_ 1
#elif defined(PLATFORM_WINDOWS)
# define EFLI_ENDIAN_FALLBACK_ Little
#elif __has_include(<endian.h>)
# include <endian.h>
# define EFLI_ENDIAN_GLIBC_ 1
#elif __has_include(<machine/endian.h>) || \
 defined(PLATFORM_APPLE)
# include <machine/endian.h>
# define EFLI_ENDIAN_MACH_ 1
#elif defined(__bsdi__) || defined(__DragonFly__) || \
 defined(__FreeBSD__) || __has_include(<sys/_endian.h>)
# include <sys/_endian.h>
# define EFLI_ENDIAN_SYS_ 1
#elif defined(PLATFORM_SUNOS) || defined(ARCH_MIPS) || \
 (defined(_AIX) || defined(__TOS_AIX__)) || \
 (defined(__sparc__) || defined(__sparc)) || \
 (defined(__hpux) || defined(_hpux) || defined(hpux)) || \
 (defined(__sgi) || defined(sgi))
# define EFLI_ENDIAN_FALLBACK_ Big
#else
# ifdef COMPILER_MSVC
#  pragma message Unknown architecture, defaulting to little endian!
# else
#  warning Unknown architecture, defaulting to little endian!
# endif
# define EFLI_ENDIAN_FALLBACK_ Little
#endif

namespace efl {
namespace C {
/// The implementation-defined endianness.
enum class Endianness : unsigned {
#if defined(EFLI_ENDIAN_STL_)
# undef EFLI_ENDIAN_STL_
  Little = unsigned(std::endian::little),
  Big    = unsigned(std::endian::big),
  Native = unsigned(std::endian::native),
#elif defined(EFLI_ENDIAN_GLIBC_)
# undef EFLI_ENDIAN_GLIBC_
  Little = __ORDER_LITTLE_ENDIAN__,
  Big    = __ORDER_BIG_ENDIAN__,
  Native = __BYTE_ORDER__
#elif defined(EFLI_ENDIAN_MACH_)
# undef EFLI_ENDIAN_MACH_
  Little = LITTLE_ENDIAN,
  Big    = BIG_ENDIAN,
  Native = BYTE_ORDER
#elif defined(EFLI_ENDIAN_SYS_)
# undef EFLI_ENDIAN_SYS_
  Little = _LITTLE_ENDIAN,
  Big    = _BIG_ENDIAN,
  Native = _BYTE_ORDER
#elif defined(EFLI_ENDIAN_FALLBACK_)
  Little = 1234,
  Big    = 4321,
  Native = EFLI_ENDIAN_FALLBACK_
# undef EFLI_ENDIAN_FALLBACK_
#endif
};

} // namespace C
} // namespace efl

#endif // EFL_CORE_ENDIAN_HPP
