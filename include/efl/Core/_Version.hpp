//===- Core/_Version.hpp --------------------------------------------===//
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
//  This file is a workaround for versioning info used internally.
//  Made this its own file since the macros were randomly 
//  getting undefined. Keeping this because it simplifies things.
//
//===----------------------------------------------------------------===//

#pragma once

#include <CoreCommon/ConfigCache.hpp>

// C++11 constexpr
// Always defined, as it is a library requirement.
#define EFLI_CXX11_CXPR_ constexpr

// C++14 constexpr
#if CPPVER_LEAST(14)
# define EFLI_CXX14_CXPR_ constexpr
#else
# define EFLI_CXX14_CXPR_
#endif

// C++17 constexpr
#if CPPVER_LEAST(17)
# define EFLI_CXX17_CXPR_ constexpr
#else
# define EFLI_CXX17_CXPR_
#endif

// C++20 constexpr
#if CPPVER_LEAST(20)
# define EFLI_CXX20_CXPR_ constexpr
#else
# define EFLI_CXX20_CXPR_
#endif

// C++23 constexpr
#if CPPVER_LEAST(23)
# define EFLI_CXX23_CXPR_ constexpr
#else
# define EFLI_CXX23_CXPR_
#endif

