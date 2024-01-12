//===- Core/_Builtins/Assert.hpp ------------------------------------===//
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
//  EFLI_CXPRASSERT_: Asserts usable in constant expressions.
//  EFLI_XCXPRASSERT_: Asserts only triggered in constant expressions.
//
//===----------------------------------------------------------------===//

#pragma once

#ifndef EFLH_CORE_BUILTINS_ASSERT_HPP
#define EFLH_CORE_BUILTINS_ASSERT_HPP

#include "IConfig.hpp"
#include "UndAssert.hpp"

/// Constexpr-able assert.
#if CPPVER_LEAST(14)
# define EFLI_CXPRASSERT_(...) \
  do { EFLI_CXPRASSERT_IMPL_(#__VA_ARGS__, bool(__VA_ARGS__)) } while(0)
#else
# define EFLI_CXPRASSERT_(...) \
  EFLI_CXPRASSERT_IMPL_(#__VA_ARGS__, bool(__VA_ARGS__))
#endif

// Explicitly constexpr assert & constexpr-able assert impl.
#if (EFLI_HAS_CXPREVAL_ == 1) && CPPVER_LEAST(14)
# define EFLI_XCXPRASSERT_(...) \
  EFLI_CXPRASSERT_(__VA_ARGS__)
# define EFLI_CXPRASSERT_IMPL_(msg, expr) \
  if(EFLI_EXPECT_FALSE_(EFLI_CXPREVAL_() && \
    !(expr))) EFL_UNREACHABLE(); \
  else if(EFLI_EXPECT_FALSE_(!EFLI_CXPREVAL_() && \
    !(expr))) EFLI_UNDASSERT_(msg);
#elif CPPVER_LEAST(14)
# define EFLI_XCXPRASSERT_(...) (void)(0)
# define EFLI_CXPRASSERT_IMPL_(msg, expr) \
  if(EFLI_EXPECT_FALSE_(expr)) EFLI_UNDASSERT_(msg);
#else
# define EFLI_XCXPRASSERT_(...)
# define EFLI_CXPRASSERT_IMPL_(msg, expr) \
  (EFLI_EXPECT_TRUE_(expr) ? (void)(0) \
    : [](){EFLI_UNDASSERT_(msg);}());
#endif

#if COMPILER_DEBUG == 1
# define EFLI_DBGASSERT_(...) \
  EFLI_CXPRASSERT_(__VA_ARGS__)
#else
# define EFLI_DBGASSERT_(...) \
  EFLI_XCXPRASSERT_(__VA_ARGS__)
#endif // Debug Check

#endif // EFLH_CORE_BUILTINS_ASSERT_HPP
