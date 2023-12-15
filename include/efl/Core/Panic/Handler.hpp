//===- Core/Panic/Handler.hpp ---------------------------------------===//
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
//  This file provides the definitions used for panic handling.
//
//===----------------------------------------------------------------===//

#pragma once

#ifndef EFL_CORE_PANIC_HANDLER_HPP
#define EFL_CORE_PANIC_HANDLER_HPP

#include <efl/Core/Fundamental.hpp>
#include <efl/Core/_Builtins.hpp>

namespace efl {
namespace C {
namespace H {
  enum class PanicType : u32 {
    
  };

  using PanicHandlerType = void(*)();
} // namespace H

struct PanicBase {

};

void panic_();

} // namespace C
} // namespace efl

#endif // EFL_CORE_PANIC_HANDLER_HPP
