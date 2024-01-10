//===- Core/_Cxx11Assert.hpp ----------------------------------------===//
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
//  This file adds some utilities for using assertions in constexpr
//  functions before C++14.
//
//===----------------------------------------------------------------===//

#pragma once

#ifndef EFLH_CORE_CXX11ASSERT_HPP
#define EFLH_CORE_CXX11ASSERT_HPP

#include <cassert>
#include "_Builtins.hpp"

#if CPPVER_LEAST(14) || \
 (defined(COMPILER_GCC) || defined(COMPILER_CLANG))
# define EFLI_CXPR11ASSERT_(...) \
  EFLI_DBGASSERT_(__VA_ARGS__)
#else
# define EFLI_CXPR11ASSERT_(...) (void)(0)
#endif

#if CPPVER_MATCH(11) && \
 (defined(COMPILER_GCC) || defined(COMPILER_CLANG))
# define EFLI_CXPR11ASSERT_PROLOGUE_ \
  LLVM_IGNORED("-Wc++14-extensions") \
  GNU_IGNORED("-Wc++14-extensions")
# define EFLI_CXPR11ASSERT_EPILOGUE_ \
  GNU_POP() LLVM_POP()
#else
# define EFLI_CXPR11ASSERT_PROLOGUE_
# define EFLI_CXPR11ASSERT_EPILOGUE_
#endif

#endif // EFLH_CORE_CXX11ASSERT_HPP
