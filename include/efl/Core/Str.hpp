//===- Core/Str.hpp -------------------------------------------------===//
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
//  Provides a mimalloc-ed string implementation.
//
//===----------------------------------------------------------------===//

#pragma once

#ifndef EFL_CORE_STR_HPP
#define EFL_CORE_STR_HPP

#include <string>
#include "MimAllocator.hpp"

namespace efl {
namespace C {
/// `std::basic_string` alias utilizing mimalloc.
template <typename CharType,
  typename Traits = std::char_traits<CharType>,
  typename Allocator = MimAllocator<CharType>>
using BasicStr = std::basic_string<
  CharType, Traits, Allocator>;

using Str = BasicStr<char>;
using WStr = BasicStr<wchar_t>;

} // namespace C
} // namespace efl

#endif // EFL_CORE_STR_HPP
