//===- Core/Poly.hpp ------------------------------------------------===//
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
//  This file implements stack-allocated type-checked polymorphic
//  objects. It allows you to avoid dynamic allocation.
//
//===----------------------------------------------------------------===//

#pragma once

#ifndef EFL_CORE_POLY_HPP
#define EFL_CORE_POLY_HPP

#include "AlignedStorage.hpp"
#include "Traits.hpp"
#include "_Cxx11Assert.hpp"
#include "_Version.hpp"

#if CPPVER_LEAST(20)
# include <concepts>
#endif

EFLI_CXPR11ASSERT_PROLOGUE_

namespace efl {
namespace C {
namespace H {
#if CPPVER_LEAST(20)
  template <typename Base, typename...Derived>
  concept all_derived = (... && 
    std::derived_from<Derived, Base>);
#endif // Concept Check (C++20)


} // namespace H

template <typename Base, typename...Derived>
MEflOptRequires(has_virtual_destructor_v<Base> && 
  H::all_derived<Base, Derived...>)
struct Poly {
#if CPPVER_MOST(17)
  static_assert(conjunction<is_base_of<Base, Derived>...>::value,
    "Types must be derived from Base.");
#endif
  using StorageType = AlignedUnion<Base, Derived...>;
  static constexpr auto emptyState = ~H::SzType(0);

public:
  static CONSTEVAL H::SzType
   Size() const NOEXCEPT {
    return sizeof...(Derived);
  }

  constexpr bool holdsAny() const NOEXCEPT {
    return !(this->id_ == emptyState);
  }

private:
  StorageType data_ { };
  H::SzType id_ = emptyState;
};

} // namespace C
} // namespace efl

EFLI_CXPR11ASSERT_EPILOGUE_

#endif // EFL_CORE_POLY_HPP
