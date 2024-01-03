//===- Core.hpp -----------------------------------------------------===//
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
//  This file includes the entire core library. I would recommend
//  including the specific files you need, for both speed and clarity,
//  but it's here if you want it :)
//
//===----------------------------------------------------------------===//

#pragma once

#ifndef EFL_CORE_HPP
#define EFL_CORE_HPP

#include <CoreCommon/ConfigCache.hpp>

//=== C++11 Includes ===//

#include "Core/AlignedStorage.hpp"
#include "Core/Binding.hpp"
#include "Core/Casts.hpp"
#include "Core/Fundamental.hpp"
#include "Core/Mtx.hpp"
#include "Core/Option.hpp"
// #include "Core/Panic.hpp" // Unfinished
#include "Core/Poly.hpp"
#include "Core/Preload.hpp"
#include "Core/Ref.hpp"
#include "Core/Result.hpp"
#include "Core/StrRef.hpp"
#include "Core/Traits.hpp"
#include "Core/Unwrap.hpp"

//=== Other Includes ===//

#if CPPVER_LEAST(17)
#include "Core/Tuple.hpp"
#endif

#endif // EFL_CORE_HPP
