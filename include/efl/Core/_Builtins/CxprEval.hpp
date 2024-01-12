//===- Core/_Builtins/CxprEval.hpp ----------------------------------===//
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
//  This file implements macros for dealing with nonstandard uses
//  of is_constant_evaluated. Can use builtins/library macros.
//
//===----------------------------------------------------------------===//

#pragma once

#ifndef EFLH_CORE_BUILTINS_CXPREVAL_HPP
#define EFLH_CORE_BUILTINS_CXPREVAL_HPP

#include "IConfig.hpp"

#if (__cpp_if_consteval >= 202106L)
# include <type_traits>
# define EFLI_HAS_CXPREVAL_ 1
# define EFLI_CXPREVAL_() ::std::is_constant_evaluated()
#elif defined(EFLI_STL_GLIBCXX_)
# if _GLIBCXX_HAVE_IS_CONSTANT_EVALUATED
#  define EFLI_HAS_CXPREVAL_ 1
#  define EFLI_CXPREVAL_() ::std::__is_constant_evaluated()
# endif
#endif // Constant evaluation check

#ifndef EFLI_HAS_CXPREVAL_
# if __has_builtin(__builtin_is_constant_evaluated)
#  define EFLI_HAS_CXPREVAL_ 1
#  define EFLI_CXPREVAL_() __builtin_is_constant_evaluated()
# else
#  define EFLI_HAS_CXPREVAL_ 0
#  define EFLI_CXPREVAL_() false
# endif
#endif // Constant evaluation check fallback

#endif // EFLH_CORE_BUILTINS_CXPREVAL_HPP
