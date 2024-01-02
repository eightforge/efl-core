//===- Core/OverloadSet.hpp -----------------------------------------===//
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
//  This file implements an object which derives from 
//  multiple base's operator(). This creates an overloaded functor
//  which can be used for many different things.
//
//===----------------------------------------------------------------===//

#pragma once

#ifndef EFL_CORE_OVERLOADSET_HPP
#define EFL_CORE_OVERLOADSET_HPP

#include "_Version.hpp"

namespace efl {
namespace C {
/// Wrapper for generic overloads of `operator()`.
template <typename...Types>
struct OverloadSet;

#if CPPVER_LEAST(17)
template <typename...TT>
struct OverloadSet : TT... {
  using TT::operator()...;
};
#else
// TODO: Switch to a block-based approach
# include "OverloadSet.cxx14.mac"
#endif

/// Generates an `OverloadSet` of types applied to a template.
template <template <typename> class Tmpl, typename...Types>
using TypeOverloadSet = OverloadSet<Tmpl<Types>...>;

/// Generates an `OverloadSet` of values applied to a template.
template <typename ValueType, 
  template <ValueType> class Tmpl, ValueType...Values>
using ValueOverloadSet = OverloadSet<Tmpl<Values>...>;

#if CPPVER_LEAST(20)
/// Generates an `OverloadSet` of values applied to a template.
template <template <auto> class Tmpl, auto...Values>
using AutoOverloadSet = OverloadSet<Tmpl<Values>...>;
#endif

} // namespace C
} // namespace efl

#endif // EFL_CORE_OVERLOADSET_HPP
