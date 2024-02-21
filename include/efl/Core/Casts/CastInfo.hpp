//===- Core/Casts/CastInfo.hpp --------------------------------------===//
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
//  This file implements LLVM-style CastInfo<...>. 
//  This allows us to do more complex and efficient cast checks.
//
//===----------------------------------------------------------------===//

#pragma once

#ifndef EFL_CORE_CASTS_CASTINFO_HPP
#define EFL_CORE_CASTS_CASTINFO_HPP

#include <efl/Core/_Builtins.hpp>
#include <efl/Core/Traits.hpp>
#include <efl/Core/_Fwd/Box.hpp>
#include <efl/Core/_Fwd/Option.hpp>

namespace efl {
namespace C {
namespace H {
  /// Core implementation of isa<...>.
  /// Specialize this for custom behaviour.
  template <typename To, typename From,
    typename Enable = void>
  struct IIsa {
    ALWAYS_INLINE static bool
     Do(const From& V) { return To::Isa(&V); }
  };

  /// Allow unchecked upcasts.
  template <typename To, typename From>
  struct IIsa<To, From, 
   enable_if_t<is_base_of<To, From>::value>> {
    ALWAYS_INLINE static bool
     Do(const From&) { return true; }
  };

  // Dispatch

  template <typename To, typename From>
  struct IIsaHL {
    ALWAYS_INLINE static bool
     Do(const From& V) {
      return IIsa<To, From>::Do(V);
    }
  };

  template <typename To, typename From>
  struct IIsaHL<To, const From> {
    ALWAYS_INLINE static bool
     Do(const From& V) {
      return IIsa<To, From>::Do(V);
    }
  };

  template <typename To, typename From>
  struct IIsaHL<To, From*> {
    ALWAYS_INLINE static bool
     Do(const From* P) {
      $assert(P, "isa<> used on nullptr!");
      return IIsa<To, From>::Do(*P);
    }
  };

  template <typename To, typename From>
  struct IIsaHL<To, const From*> {
    ALWAYS_INLINE static bool
     Do(const From* P) {
      $assert(P, "isa<> used on nullptr!");
      return IIsa<To, From>::Do(*P);
    }
  };

  template <typename To, typename From>
  struct IIsaHL<To, From* const>
   : IIsaHL<To, From*> { };

  template <typename To, typename From, typename A>
  struct IIsaHL<To, Box<From, A>> {
    ALWAYS_INLINE static bool
     Do(const Box<From, A>& P) {
      $assert(P, "isa<> used on nullptr!");
      return IIsaHL<To, From>::Do(*P);
    }
  };

  //=== Return Type ===//

  /// Deduce a cast's return type.
  template <typename To, typename From>
  struct ICastRetTy { using Ret = To&; };

  template <typename To, typename From>
  struct ICastRetTy<To, const From> 
  { using Ret = const To&; };

  template <typename To, typename From>
  struct ICastRetTy<To, From*> 
  { using Ret = To*; };

  template <typename To, typename From>
  struct ICastRetTy<To, const From*> 
  { using Ret = const To*; };

  template <typename To, typename From>
  struct ICastRetTy<To, From* const> { 
    using Ret = typename 
      ICastRetTy<To, From*>::Ret;
  };

  template <typename To, typename From, typename A>
  struct ICastRetTy<To, Box<From, A>> { 
  private:
    using PtrType = typename
      ICastRetTy<To, From*>::Ret;
    using RawType = remove_pointer_t<PtrType>;
  public:
    using Ret = Box<RawType, A>;
  };

  template <typename To, typename From>
  using CastRetTy = typename
    ICastRetTy<To, From>::Ret;
  
  template <typename To, typename From>
  using RmRefCastRetTy =
    remove_reference_t<CastRetTy<To, From>>; 
  
  // Conversion

  template <typename To, typename From>
  struct CastConvert {
    /// Convert from `[Qual] X&` -> `Y*` -> `Y&`.
    static CastRetTy<To, From> Do(const From& V) {
      return *(RmRefCastRetTy<To, From>*)
        &const_cast<From&>(V);
    }
  };

  template <typename To, typename From>
  struct CastConvert<To, From*> {
    /// Convert from `[Qual] X*` -> `Y*`.
    static CastRetTy<To, From*> Do(const From* P) {
      return (CastRetTy<To, From>) const_cast<From*>(P);
    }
  };
} // namespace H

template <typename To, typename From,
  typename Enable = void>
struct CastCheck {
  inline static bool IsPossible(const From& V) {
    return H::IIsaHL<To, const From>::Do(V);
  }
};

/// Allow unchecked upcasts.
template <typename To, typename From>
struct CastCheck<To, From, 
 enable_if_t<is_base_of<To, From>::value>> {
  constexpr static bool
   IsPossible(const From&) { return true; }
};

template <typename To, typename From>
struct CastCheck<To, Option<From>> {
  using NoRef = remove_reference_t<From>;
  inline static bool IsPossible(const Option<From>& O) {
    $assert(O, "isa<> used on nullopt!");
    return H::IIsaHL<To, const NoRef>::Do(*O);
  }
};

//=== Cast Traits ===//

/// The default failure adaptor for `T*`.
template <typename To> 
struct NullableValCastFailed {
  static To CastFailed() { return To(nullptr); }
};

template <typename To, typename From, class Impl>
struct DefaultDoFailableCast {
  static To DoFailableCast(From f) {
    if (!Impl::IsPossible(f))
      return Impl::CastFailed();
    return Impl::DoCast(f);
  }
};

namespace H {
  template <class OptImpl, class Default>
  using CastSelfType = conditional_t<
    is_void<OptImpl>::value, Default, OptImpl>;
} // namespace H

// Cast Implementations

/// Implements casts from `From*` -> `To`.
template <typename To, typename From,
  class Impl = void>
struct ValFromPtrCast : CastCheck<To, From*>,
 NullableValCastFailed<To>,
 DefaultDoFailableCast<To, From*,
   H::CastSelfType<Impl, ValFromPtrCast<To, From>>> {
public:
  inline static To DoCast(From* P) { return To(P); }
};

/// Implements "casts" from `Box<From, ...>` -> `To`.
/// Consumes the `Box` when the cast is valid.
template <typename To, typename From, typename A,
  class Impl = void>
struct BoxCast : CastCheck<To, From*> {
  using SelfType = H::CastSelfType<Impl, BoxCast<To, From, A>>;
  using RetType = Box<H::RmRefCastRetTy<To, From>, A>;
public:
  inline static RetType DoCast(Box<From, A>&& B) {
    using ElemType = typename RetType::element_type;
    /// Safe because the allocator is type-checked.
    return RetType::NewFromPtr((ElemType*)B.release());
  }

  inline static RetType
   DoFailableCast(Box<From, A>& B) {
    if (!SelfType::IsPossible(B.get()))
      return CastFailed();
    return DoCast(H::cxpr_move(B));
  }

  inline static RetType CastFailed() {
    return RetType(nullptr);
  }
};

/// Implements casts from `From` -> `Option<To>`.
template <typename To, typename From,
  class Impl = void>
struct OptionCast : CastCheck<To, From>,
 DefaultDoFailableCast<Option<To>, From,
   H::CastSelfType<Impl, OptionCast<To, From>>> {
  using RetType = Option<To>;
public:
  inline static RetType CastFailed() { return RetType(); }
  inline static RetType DoCast(const From& V) { return RetType(V); }
};

/// Implements a trait which strips `const` from some `T`,
/// then invokes the supplied implementation.
template <typename To, typename From, class Impl>
struct FwdStrippedQualCast {
  /// Removes any qualifiers from a possibly pointer type.
  using RawFrom = remove_cv_t<remove_pointer_t<From>>;
  /// Get `From&/From*` with stripped qualifiers.
  using StrippedFrom = conditional_t<
    is_pointer<From>::value, RawFrom*, RawFrom&>;
private:
  ALWAYS_INLINE static StrippedFrom Strip(const From& V) {
    return const_cast<StrippedFrom>(V);
  }
public:
  inline static bool IsPossible(const From& V) {
    return Impl::IsPossible(Strip(V));
  }
  inline static auto DoCast(const From& V)
   -> decltype(Impl::DoCast(Strip(V))) {
    return Impl::DoCast(Strip(V));
  }
  inline static auto DoFailableCast(const From& V)
   -> decltype(Impl::DoFailableCast(Strip(V))) {
    return Impl::DoFailableCast(Strip(V));
  }
  inline static auto CastFailed()
   -> decltype(Impl::CastFailed()) {
    return Impl::CastFailed();
  }
};

/// Implements a trait which converts `T&` -> `T*`,
/// then invokes the supplied `T*` implementation.
template <typename To, typename From, class Impl>
struct FwdToPtrCast {
  inline static bool IsPossible(const From& V) {
    return Impl::IsPossible(&V);
  }
  inline static auto DoCast(const From& V)
   -> decltype(*Impl::DoCast(&V)) {
    return *Impl::DoCast(&V);
  }
};

//=== CastInfo ===//

template <typename To, typename From,
  typename Enable = void>
struct CastInfo : CastCheck<To, From> {
  using SelfType = CastInfo<To, From, Enable>;
  using RetType  = H::CastRetTy<To, From>;
public:
  inline static RetType DoCast(const From& V) {
    return H::CastConvert<To, From>::Do(
      const_cast<From&>(V));
  }
  inline static RetType DoFailableCast(const From& V) {
    if (!SelfType::IsPossible(V))
      return CastFailed();
    return DoCast(V);
  }
  inline static RetType CastFailed() {
    return RetType(nullptr);
  }
};

// Specializations

/// Specialization for `Box<...>` types.
template <typename To, typename From, typename A>
struct CastInfo<To, Box<From, A>> 
 : BoxCast<To, From, A> { };

/// Specialization for `Option<...>` types.
template <typename To, typename From>
struct CastInfo<To, Option<From>> 
 : OptionCast<To, From> { };

//=== Value Checking ===//

template <typename T>
using IsNullable = H::Or<
  is_pointer<T>, is_constructible<T, decltype(nullptr)>>;

template <typename T, 
  typename Enable = void>
struct CheckValue {
  using UnwrappedType = T;
  constexpr static bool HasValue(T&) { return true; }
  constexpr static T& DoUnwrap(T& V) { return V; }
};

template <typename T> struct CheckValue<Option<T>> {
  using UnwrappedType = T;
  inline static bool HasValue(Option<T>& O) { 
    return O.hasValue();
  }
  inline static T& DoUnwrap(Option<T>& O) { return *O; }
};

template <typename T> 
struct CheckValue<T, enable_if_t<IsNullable<T>::value>> {
  using UnwrappedType = T;
  inline static bool HasValue(T& V) { 
    return V != T(nullptr);
  }
  inline static T& DoUnwrap(T& V) { return V; }
};

namespace H {
  template <typename T>
  inline bool HasValue(const T& V) {
    return CheckValue<T>::HasValue(const_cast<T&>(V));
  }

  template <typename T>
  inline auto DoUnwrap(const T& V)
   -> decltype(CheckValue<T>::DoUnwrap(Decl<T&>())) {
    return CheckValue<T>::DoUnwrap(const_cast<T&>(V));
  }
} // namespace H

} // namespace C
} // namespace efl

#endif // EFL_CORE_CASTS_CASTINFO_HPP