//===- Core/Poly.hpp ------------------------------------------------===//
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
//  This file implements stack-allocated type-checked polymorphic
//  objects. It allows you to avoid dynamic allocation.
//
//===----------------------------------------------------------------===//

#pragma once

#ifndef EFL_CORE_POLY_HPP
#define EFL_CORE_POLY_HPP

#include "AlignedStorage.hpp"
#include "Casts.hpp"
#include "OverloadSet.hpp"
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
  template <typename T, typename...UU>
  concept one_of = (... || 
    std::same_as<T, UU>);

  template <typename Base, typename...Derived>
  concept all_derived = (... && 
    std::derived_from<Derived, Base>);
#endif // Concept Check (C++20)

  template <typename T, H::SzType I>
  struct PolyNode {
    AGGRESSIVE_INLINE constexpr H::SzType 
     operator()(T* base) const NOEXCEPT 
    { return I; }

    AGGRESSIVE_INLINE constexpr H::SzType 
     operator()(const T* base) const NOEXCEPT
    { return I; }

    template <H::SzType N>
    ALWAYS_INLINE EFLI_CXX20_CXPR_ char
     operator()(TypeC<T>, H::SzType n, ubyte(&ptr)[N]) const NOEXCEPT {
      if(n == I) {
        launder_cast<T>(ptr)->~T();
        return true;
      }
      return false;
    }
  };

  template <typename Seq, typename...>
  struct TPolyNodes {
    COMPILE_FAILURE(Seq, "Requires a SzSeq.");
  };

  template <SzType...II, typename...TT>
  struct MSVC_EMPTY_BASES TPolyNodes<SzSeq<II...>, TT...> 
   : OverloadSet<PolyNode<TT, II>...> { };
  
  template <typename Base, typename...Derived>
  using PolyNodes = TPolyNodes<
    MkSzSeq<sizeof...(Derived) + 1>, Base, Derived...>;
  
  template <typename T, typename...TT>
  using MatchesAny = disjunction<is_same<T, TT>...>;
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
  using NodeValuesType = H::PolyNodes<Base, Derived...>;
  static constexpr auto emptyState = ~H::SzType(0);

protected:
  template <typename...Args>
  ALWAYS_INLINE static constexpr auto
   NodeValues(Args&&...args) NOEXCEPT
   -> decltype(H::Decl<NodeValuesType&&>()(
     H::Decl<Args>()...)) {
    return NodeValuesType{}(
      H::cxpr_forward<Args>(args)...);
  }

  template <typename T>
  static constexpr bool matchesAny =
    H::MatchesAny<T, Base, Derived...>::value;
  
  template <typename T>
  static constexpr H::SzType idValue =
    NodeValues(static_cast<T*>(nullptr));

public:
  constexpr Poly() = default;
  // TODO: Implement these
  Poly(const Poly&) = delete;
  Poly(Poly&&) = delete;
  ~Poly() NOEXCEPT { this->clear(); }

  template <typename T, MEflEnableIf(matchesAny<T> && 
    is_copy_constructible<T>::value)>
  Poly(const T& value) : id_(idValue<T>) {
    MAYBE_UNUSED auto* p = 
      new (data_.data) T(value);
  }

  template <typename T, MEflEnableIf(matchesAny<T> && 
    is_move_constructible<T>::value)>
  Poly(T&& value) : id_(idValue<T>) {
    MAYBE_UNUSED auto* p = 
      new (data_.data) T(std::move(value));
  }

  template <typename T, MEflEnableIf(matchesAny<T> && 
    is_copy_assignable<T>::value)>
  EFLI_CXX14_CXPR_ Poly& operator=(const T& value) {
    this->clear();
    id_ = idValue<T>;
    MAYBE_UNUSED auto* p = 
      new (data_.data) T(value);
    return *this;
  }

  template <typename T, MEflEnableIf(matchesAny<T> && 
    is_move_assignable<T>::value)>
  EFLI_CXX14_CXPR_ Poly& operator=(T&& value) {
    this->clear();
    id_ = idValue<T>;
    MAYBE_UNUSED auto* p = 
      new (data_.data) T(std::move(value));
    return *this;
  }

public:
  static constexpr H::SzType
   Size() NOEXCEPT {
    return sizeof...(Derived);
  }

  FICONSTEXPR bool holdsAny() const NOEXCEPT {
    return !(this->id_ == emptyState);
  }

  template <typename T>
  FICONSTEXPR bool holdsType() const NOEXCEPT {
    return (this->id_ == idValue<T>);
  }

  template <typename T, MEflEnableIf(matchesAny<T>)>
  AGGRESSIVE_INLINE constexpr bool
   safeHoldsType() const NOEXCEPT {
    return this->holdsType();
  }

  template <typename T, MEflEnableIf(!matchesAny<T>)>
  FICONSTEXPR bool safeHoldsType() const NOEXCEPT 
  { return false; }

  ALWAYS_INLINE auto asBase() NOEXCEPT
   -> H::launder_t<Base> {
    return launder_cast<Base>(data_.data);
  }

  ALWAYS_INLINE auto asBase() const NOEXCEPT
   -> H::launder_t<const Base> {
    return launder_cast<const Base>(data_.data);
  }

  H::launder_t<Base> get() NOEXCEPT {
    return launder_cast<Base>(data_.data); 
  }

  H::launder_t<const Base> get() const NOEXCEPT {
    return launder_cast<const Base>(data_.data); 
  }

  Base* operator->() NOEXCEPT {
    EFLI_DBGASSERT_(this->holdsAny());
    return (Base*)(this->asBase());
  }

  const Base* operator->() const NOEXCEPT {
    EFLI_DBGASSERT_(this->holdsAny());
    return (const Base*)(this->asBase());
  }

  template <typename T, 
    MEflEnableIf(matchesAny<T>)>
  T* downcast()& NOEXCEPT {
    if(!this->holdsType<T>()) return nullptr;
    return launder_cast<T>(data_.data);
  }

  template <typename T, 
    MEflEnableIf(matchesAny<T>)>
  const T* downcast() const& NOEXCEPT {
    if(!this->holdsType<T>()) return nullptr;
    return launder_cast<const T>(data_.data);
  }

  template <typename T, 
    MEflEnableIf(matchesAny<T>)>
  T& downcastUnchecked()& NOEXCEPT {
    EFLI_DBGASSERT_(this->holdsType<T>());
    return *launder_cast<T>(data_.data);
  }

  template <typename T, 
    MEflEnableIf(matchesAny<T>)>
  const T& downcastUnchecked() const& NOEXCEPT {
    EFLI_DBGASSERT_(this->holdsType<T>());
    return *launder_cast<const T>(data_.data);
  }

  template <typename T, 
    MEflEnableIf(matchesAny<T>)>
  T&& downcastUnchecked()&& NOEXCEPT {
    EFLI_DBGASSERT_(this->holdsType<T>());
    return std::move(*launder_cast<T>(data_.data));
  }

  void clear() NOEXCEPT {
    this->clear_();
    id_ = emptyState;
  }

private:
  ALWAYS_INLINE void clear_() NOEXCEPT {
    if(!holdsAny()) return;
    else if(this->id_ == 0) {
      this->asBase()->~Base();
      return;
    }

# if CPPVER_LEAST(17)
    (void)((... || NodeValues(
      H::TypeC<Derived>{}, id_, data_.data)));
# else
    bool dd[sizeof...(Derived) + 1] = { 
      (id_ == idValue<Base> && 
       NodeValues(H::TypeC<Derived>{}, 
        id_, data_.data))...
    };
    (void)dd;
# endif
  }

  template <typename T>
  bool assignPass(const Poly& rhs) {
    bool holds = rhs.holdsType<T>();
    if(holds) {
      MAYBE_UNUSED auto* p = 
       new (data_.data) 
       T(rhs.template downcast<T>());
    }
    return holds;
  }

  template <typename T>
  bool assignPass(Poly&& rhs) {
    bool holds = rhs.holdsType<T>();
    if(holds) {
      MAYBE_UNUSED auto* p = 
       new (data_.data) 
       T(std::move(rhs.template downcast<T>()));
    }
    return holds;
  }

private:
  StorageType data_ { };
  H::SzType id_ = emptyState;
};

} // namespace C
} // namespace efl

EFLI_CXPR11ASSERT_EPILOGUE_

#endif // EFL_CORE_POLY_HPP
