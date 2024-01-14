//===- Core/_Builtins/FI128.hpp -------------------------------------===//
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

#ifndef EFLH_CORE_BUILTINS_FI128_HPP
#define EFLH_CORE_BUILTINS_FI128_HPP

#include "IConfig.hpp"

// __int128 check
#ifndef EFLI_HAS_I128_
# if defined(__SIZEOF_INT128__)
#  define EFLI_HAS_I128_ 1
# elif defined(EFLI_STL_GLIBCXX_) && \
 !defined(__STRICT_ANSI__) && \
 defined(__GLIBCXX_TYPE_INT_N_0)
// GCC supports __int128
#  define EFLI_HAS_I128_ 1
# elif defined(EFLI_STL_LIBCPP_) && \
 !defined(_LIBCPP_HAS_NO_INT128)
// Clang supports __int128
#  define EFLI_HAS_I128_ 1
# else
#  define EFLI_HAS_I128_ 0
# endif
#endif

// __float128 check
#ifndef EFLI_HAS_F128_
# if defined(EFLI_STL_GLIBCXX_) && \
 !defined(__STRICT_ANSI__) && \
 defined(_GLIBCXX_USE_FLOAT128)
// GCC supports __float128
#  define EFLI_HAS_F128_ 1
# elif defined(EFLI_STL_LIBCPP_) && \
 (defined(_M_X64) || defined(__x86_64__))
// Clang supports __float128
#  define EFLI_HAS_F128_ 1
# else
#  define EFLI_HAS_F128_ 0
# endif
#endif

#endif // EFLH_CORE_BUILTINS_FI128_HPP
