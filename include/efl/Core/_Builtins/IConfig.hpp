//===- Core/_Builtins/IConfig.hpp -----------------------------------===//
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
//  This file implements basic internal configurations.
//
//===----------------------------------------------------------------===//

#pragma once

#ifndef EFLH_CORE_BUILTINS_ICONFIG_HPP
#define EFLH_CORE_BUILTINS_ICONFIG_HPP

#include <CoreCommon/ConfigCache.hpp>
#include <CoreCommon/Multithreaded.hpp>

#if CPPVER_LEAST(20) || __has_include(<version>)
# include <version>
#endif

#if __has_include(<__config>)
# include <__config>
# define EFLI_STL_LIBCPP_ 1
#elif __has_include(<bits/c++config.h>)
# include <bits/c++config.h>
# define EFLI_STL_GLIBCXX_ 1
#elif __has_include(<yvals_core.h>)
# include <yvals_core.h>
# define EFLI_STL_MSVC_ 1
#endif

#if defined(PLATFORM_APPLE) || \
 __has_include(<sys/cdefs.h>)
# include <sys/cdefs.h>
#endif

#ifdef EFLI_STL_MSVC_
# define EFLI_BEGIN_CSTD_ _CRT_BEGIN_C_HEADER
# define EFLI_END_CSTD_ _CRT_END_C_HEADER
#else
# define EFLI_BEGIN_CSTD_ extern "C" {
# define EFLI_END_CSTD_ }
#endif

#if (__cpp_deduction_guides >= 201907L)
# define EFLI_HAS_ALIAS_UCTAD_ 1
#else
# define EFLI_HAS_ALIAS_UCTAD_ 0
#endif

#if EFL_HAS_CPP_ATTRIBUTE(gnu::cold) && \
  EFL_HAS_CPP_ATTRIBUTE(gnu::noinline)
# define EFLI_COLD_PATH_ [[gnu::cold, gnu::noinline]]
#elif defined(__GNUC__)
# define EFLI_COLD_PATH_ __attribute__((cold, noinline))
#else
# define EFLI_COLD_PATH_ NOINLINE
#endif

#endif // EFLH_CORE_BUILTINS_ICONFIG_HPP
