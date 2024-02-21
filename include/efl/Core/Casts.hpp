//===- Core/Casts.hpp -----------------------------------------------===//
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
//  This file defines some casting helper functions.
//
//===----------------------------------------------------------------===//

#pragma once

#ifndef EFL_CORE_CASTS_HPP
#define EFL_CORE_CASTS_HPP

#include "Traits.hpp"
#include "Casts/CastInfo.hpp"
#include "Casts/Launder.hpp"
#include "Casts/Pun.hpp"

#if EFLI_BIT_CAST_ == 1
# define EFLI_PUNCAST_CXPR_ constexpr
#else
# define EFLI_PUNCAST_CXPR_
#endif

// TODO: Add isa, dyn_cast, etc. (handle casts?)

/// Uses the available "laundering" function.
#define EFL_LAUNDER(...) EFLI_LAUNDER_(__VA_ARGS__)

namespace efl {
namespace C {
/// `static_cast`s a value to `bool`.
template <typename T>
FICONSTEXPR bool bool_cast(T& V) noexcept(
 noexcept(static_cast<bool>(V))) {
  return static_cast<bool>(V);
}

/// `static_cast`s a moved value to `bool`.
template <typename T>
FICONSTEXPR bool bool_cast(T&& V) noexcept(
 noexcept(static_cast<bool>(H::cxpr_move(V)))) {
  return static_cast<bool>(H::cxpr_move(V));    
}

/// Wraps the implementation of `launder`.
/// Uses a volatile pointer wrapper as a fallback.
template <typename T>
EFLI_LAUNDERCAST_CXPR_ auto
 launder(T* V) -> H::launder_t<T> {
  return H::launder_wrap(V);
}

/// Identical to `std::launder(V)`.
template <typename T>
EFLI_LAUNDERCAST_CXPR_ auto 
 launder_cast(T* V) -> H::launder_t<T> {
  return H::launder_wrap(V);
}

template <typename T>
EFLI_LAUNDERCAST_CXPR_ auto 
 launder_cast(void* P) -> H::launder_t<T> {
  return static_cast<H::launder_t<T>>(P);
}

/// Converts the argument to `T*` and invokes `H::launder_wrap`.
template <typename T>
EFLI_LAUNDERCAST_CXPR_ auto 
 launder_cast(ubyte* raw) -> H::launder_t<T> {
  T* data = reinterpret_cast<T*>(raw);
  return H::launder_wrap(data);
}

/// Converts the argument to `T*` and invokes `H::launder_wrap`.
template <typename T>
EFLI_LAUNDERCAST_CXPR_ auto 
 launder_cast(std::uintptr_t raw) -> H::launder_t<T> {
  T* data = reinterpret_cast<T*>(raw);
  return H::launder_wrap(data);
}

/// VERY DANGEROUS!!! Use only when absolutely necessary
/// (eg. std::bit_cast, std::memcpy, etc. when possible).
template <typename T, typename U>
EFLI_PUNCAST_CXPR_ T pun_cast(U u) {
  MEflESAssert(std::is_trivially_copyable<T>::value &&
    std::is_trivially_copyable<U>::value);
  MEflESAssert(sizeof(T) == sizeof(U));
  return H::PunHelper<T, U>(u).get();
}

#if CPPVER_LEAST(20)
using std::bit_cast;
#endif

/// Casts enum to its underlying type.
template <typename E>
FICONSTEXPR underlying_type_t<E>
 underlying_cast(E e) NOEXCEPT {
  static_assert(is_enum<E>::value, 
    "underlying_cast can only be called with enums.");
  return H::to_underlying(e);
}

/// Converts signed integer to unsigned.
template <typename T, MEflEnableIf(is_signed<T>::value)>
FICONSTEXPR make_unsigned_t<T> unsigned_cast(T V) NOEXCEPT {
  return static_cast<make_unsigned_t<T>>(V);
}

/// Already unsigned, noop.
template <typename T, MEflEnableIf(is_unsigned<T>::value)>
FICONSTEXPR T unsigned_cast(T V) NOEXCEPT { return V; }

//=== LLVM Style Casts ===//

// isa<...>(V):
// Returns true if `Val` is an instance of the input types.

#if CPPVER_LEAST(17)
template <typename To, typename From>
ALWAYS_INLINE bool iisa(const From& V) {
  // Possibly use LLVM_TAIL here?
  return CastInfo<To, const From>::IsPossible(V);
}

template <typename To, typename...Tos, typename From>
NODISCARD inline bool isa(const From& V) {
  return (iisa<To>(V) || ... || iisa<Tos>(V));
}
#else
template <typename To, typename From>
NODISCARD inline bool isa(const From& V) {
  return CastInfo<To, const From>::IsPossible(V);
}

template <typename Curr, typename Next,
  typename...Tails, typename From>
NODISCARD inline bool isa(const From& V) {
  return isa<Curr>(V) || isa<Next, Tails...>(V);
}
#endif // C++17 Check (Folds)

// checked_isa<...>(V):
// Returns false if `!HasValue(V)`, or returns `isa<...>(V)`.

template <typename...Tos, typename From>
NODISCARD inline bool checked_isa(const From& V) {
  MEflESAssert(sizeof...(Tos) > 0);
  if (!H::HasValue(V))
    return false;
  return isa<Tos...>(V);
}

template <typename To, typename From>
using CastFailType = decltype(
  CastInfo<To, From>::CastFailed());

// cast<...>(V):
// Casts to the input type, asserts when input is null.

template <typename To, typename From>
NODISCARD inline auto cast(From& V)
 -> decltype(CastInfo<To, From>::DoCast(V)) {
  $assert(isa<To>(V), "cast<T>() argument is invalid!");
  return CastInfo<To, From>::DoCast(V);
}

template <typename To, typename From>
NODISCARD inline auto cast(const From& V)
 -> decltype(CastInfo<To, const From>::DoCast(V)) {
  $assert(isa<To>(V), "cast<T>() argument is invalid!");
  return CastInfo<To, const From>::DoCast(V);
}

template <typename To, typename From>
NODISCARD inline auto cast(From* P)
 -> decltype(CastInfo<To, From*>::DoCast(P)) {
  $assert(isa<To>(P), "cast<T>() argument is invalid!");
  return CastInfo<To, From*>::DoCast(P);
}

template <typename To, typename From, typename A>
NODISCARD inline auto cast(Box<From, A>&& B)
 -> decltype(CastInfo<To, Box<From, A>>::DoCast(std::move(B))) {
  $assert(isa<To>(B), "cast<T>() argument is invalid!");
  return CastInfo<To, Box<From, A>>::DoCast(std::move(B));
}

// checked_cast<...>(V):
// Casts to the input type or returns null value.

template <typename To, typename From>
NODISCARD inline CastFailType<To, From>
 checked_cast(From& V) {
  if (!H::HasValue(V))
    return CastInfo<To, From>::CastFailed();
  $assert(isa<To>(V), "checked_cast<T>() argument is invalid!");
  return cast<To>(H::DoUnwrap(V));
}

template <typename To, typename From>
NODISCARD inline CastFailType<To, const From>
 checked_cast(const From& V) {
  if (!H::HasValue(V))
    return CastInfo<To, const From>::CastFailed();
  $assert(isa<To>(V), "checked_cast<T>() argument is invalid!");
  return cast<To>(H::DoUnwrap(V));
}

template <typename To, typename From>
NODISCARD inline CastFailType<To, From*>
 checked_cast(From* P) {
  if (!H::HasValue(P))
    return CastInfo<To, From*>::CastFailed();
  $assert(isa<To>(P), "checked_cast<T>() argument is invalid!");
  return cast<To>(H::DoUnwrap(P));
}

template <typename To, typename From, typename A>
NODISCARD inline Box<To, A> checked_cast(Box<From, A>&& B) {
  if (!H::HasValue(B))
    return BoxCast<To, From, A>::CastFailed();
  return BoxCast<To, From, A>::DoCast(std::move(B));
}

// dyn_cast<...>(V):
// Casts to the input type if possible, asserts when input is null.

template <typename To, typename From>
NODISCARD inline auto dyn_cast(From& V)
 -> decltype(CastInfo<To, From>::DoFailableCast(V)) {
  $assert(H::HasValue(V), "dyn_cast<T>() argument has no value!");
  return CastInfo<To, From>::DoFailableCast(V);
}

template <typename To, typename From>
NODISCARD inline auto dyn_cast(const From& V)
 -> decltype(CastInfo<To, const From>::DoFailableCast(V)) {
  $assert(H::HasValue(V), "dyn_cast<T>() argument has no value!");
  return CastInfo<To, const From>::DoFailableCast(V);
}

template <typename To, typename From>
NODISCARD inline auto dyn_cast(From* P)
 -> decltype(CastInfo<To, From*>::DoFailableCast(P)) {
  $assert(H::HasValue(P), "dyn_cast<T>() argument has no value!");
  return CastInfo<To, From*>::DoFailableCast(P);
}

template <typename To, typename From, typename A>
NODISCARD inline auto dyn_cast(Box<From, A>& B)
 -> decltype(CastInfo<To, Box<From, A>>::DoFailableCast(B)) {
  $assert(H::HasValue(B), "dyn_cast<T>() argument has no value!");
  return CastInfo<To, Box<From, A>>::DoFailableCast(B);
}

// checked_dyn_cast<...>(V):
// Casts to the input type if possible.

template <typename To, typename From>
NODISCARD inline CastFailType<To, From>
 checked_dyn_cast(From& V) {
  if (!H::HasValue(V))
    return CastInfo<To, From>::CastFailed();
  return CastInfo<To, From>::DoFailableCast(H::DoUnwrap(V));
}

template <typename To, typename From>
NODISCARD inline CastFailType<To, const From>
 checked_dyn_cast(const From& V) {
  if (!H::HasValue(V))
    return CastInfo<To, const From>::CastFailed();
  return CastInfo<To, const From>::DoFailableCast(H::DoUnwrap(V));
}

template <typename To, typename From>
NODISCARD inline CastFailType<To, From*>
 checked_dyn_cast(From* P) {
  if (!H::HasValue(P))
    return CastInfo<To, From*>::CastFailed();
  return CastInfo<To, From*>::DoFailableCast(H::DoUnwrap(P));
}

// box_cast<...>(B):
// Takes ownership of a Box<...> if cast is successful.

template <typename To, typename From, typename Alloc>
using BoxCastRetType = typename 
  CastInfo<To, Box<From, Alloc>>::RetType;

template <typename To, typename From, typename A>
NODISCARD inline BoxCastRetType<To, From, A>
 box_cast(Box<From, A>& B) {
  if (!isa<To>(B))
    return nullptr;
  return cast<To>(std::move(B));
}

template <typename To, typename From, typename A>
NODISCARD ALWAYS_INLINE BoxCastRetType<To, From, A>
 box_cast(Box<From, A>&& B) {
  return box_cast<To>(B);
}

// checked_box_cast<...>(B):
// Same as `box_cast`, but checks if input is valid.

template <typename To, typename From, typename A>
NODISCARD inline BoxCastRetType<To, From, A>
 checked_box_cast(Box<From, A>& B) {
  if (!B)
    return nullptr;
  return box_cast<To>(B);
}

template <typename To, typename From, typename A>
NODISCARD ALWAYS_INLINE BoxCastRetType<To, From, A>
 checked_box_cast(Box<From, A>&& B) {
  return checked_box_cast<To>(B);
}

} // namespace C
} // namespace efl

#undef EFLI_LAUNDERCAST_CXPR_
#undef EFLI_PUNCAST_CXPR_

#endif // EFL_CORE_CASTS_HPP
