//===- Core/Traits/Macros.hpp ---------------------------------------===//
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

#pragma once

#ifndef EFL_CORE_TRAITS_MACROS_HPP
#define EFL_CORE_TRAITS_MACROS_HPP

/**
 * Used for optional constraints. Do NOT expect this 
 * to work if the constraint is involved with overload resolution.
 */
#define MEflOptRequires(...) EFLI_REQUIRES_(__VA_ARGS__)

/// Unwraps values using statement expressions (GNU/LLVM only).
#define MEflUnwrap(obj, ...) EFLI_UNWRAP_(obj, ##__VA_ARGS__)

/// Wraps `std::enable_if` to improve readability. TODO: Remove?
#define MEflEnableIf(...) typename ::std::enable_if< \
  static_cast<bool>(__VA_ARGS__), bool>::type = true

/// Wraps `core::void_t` for trait stuffs.
#define MEflVoidT(...) ::efl::C::void_t<__VA_ARGS__>

/// Creates an object with a "unique" name.
#define MEflUniqueVar(name) EFLI_UNIQUE_VAR_(EFL_CAT(_v, name))

/**
 * Creates a trait utilizing `void_t`.
 * The passed type is always `T`.
 * @param name The name of the trait.
 * @note C++20: The trait is a concept by default.
 */
#define MEflHasTrait(name, ...) \
 EFLI_HAS_TRAIT_(name, __VA_ARGS__)

/**
 * Creates a trait utilizing `void_t`
 * where the input is a value.
 * The passed type is always `T`.
 * @param name The name of the trait.
 * @note C++20: The trait is a concept by default.
 */
#define MEflHasValueTrait(name, ...)     \
 EFLI_HAS_TRAIT_(name, decltype(__VA_ARGS__))

//=== Implementation ===//

#include <CoreCommon/ConfigCache.hpp>

#if CPPVER_LEAST(20)
# define EFLI_OREQUIRES_(...) requires(__VA_ARGS__)
#else
# define EFLI_OREQUIRES_(...)
#endif

// TODO: Add real tests for this.
#if defined(COMPILER_GCC) || defined(COMPILER_CLANG)
# define EFLI_UNWRAP_(obj, ...) \
  ({ if(!(obj).has_value()) return { __VA_ARGS__ }; (obj).unwrap(); })
#else
# define EFLI_UNWRAP(...) []() { \
  constexpr bool Unavailable = false; \
  static_assert(Unavailable, "Unable to unwrap," \
  " statement expressions are not available on your compiler."); return 0; }()
#endif

#if CPPVER_LEAST(14)
// Full trait
# define EFLI_HAS_TRAIT_(name, ...)       \
  EFLI_HAS_TRAIT_B_(name)                 \
  EFLI_HAS_TRAIT_D_(name, ##__VA_ARGS__)  \
  EFLI_HAS_TRAIT_V_(name)
#elif defined(COMPILER_GCC)
// # pragma message "Using C++14 trait extensions."
# define EFLI_HAS_TRAIT_(name, ...)       \
  EFLI_HAS_TRAIT_B_(name)                 \
  EFLI_HAS_TRAIT_D_(name, ##__VA_ARGS__)  \
  GNU_IGNORED("-Wc++14-extensions")       \
  EFLI_HAS_TRAIT_V_(name) GNU_POP()
#elif defined(COMPILER_CLANG)
// # pragma message("Using C++14 trait extensions.")
# define EFLI_HAS_TRAIT_(name, ...)       \
  EFLI_HAS_TRAIT_B_(name)                 \
  EFLI_HAS_TRAIT_D_(name, ##__VA_ARGS__)  \
  LLVM_IGNORED("-Wc++14-extensions")      \
  EFLI_HAS_TRAIT_V_(name) LLVM_POP()
#else
# define EFLI_HAS_TRAIT_(name) \
  enum EFL_CAT(name, _fail) { EFLI_HAS_TRAIT_F_(name) = 0 }; \
  static_assert(EFLI_HAS_TRAIT_F_(name), \
    "The trait " #name " requires a minimum of C++11.");
# define EFLI_HAS_TRAIT_F_(name) \
  TRICAT(e, name, RequiresCxx14)
#endif // Globals check (C++14)

#if CPPVER_LEAST(11)
// Empty trait base
# define EFLI_HAS_TRAIT_B_(name) template <typename, typename = void> \
  struct EFL_CAT(name, _impl) { static constexpr bool value = false; };
// Trait deduction specialization
# define EFLI_HAS_TRAIT_D_(name, ...) template <typename T> \
  struct EFL_CAT(name, _impl)<T, MEflVoidT(__VA_ARGS__)>        \
  { static constexpr bool value = true; };
// Trait value
# if CPPVER_LEAST(20)
#  define EFLI_HAS_TRAIT_V_(name) template <typename T> \
  concept name = EFL_CAT(name, _impl)<T>::value;
# else
#  define EFLI_HAS_TRAIT_V_(name) template <typename T> \
  GLOBAL bool name = EFL_CAT(name, _impl)<T>::value;
# endif // Concept check (C++20)
#endif // Underlying trait check

#if defined(__COUNTER__)
# define EFLI_UNIQUE_VAR_(name) \
  EFL_QCAT(name, COMPILER_LINE, _, __COUNTER__)
#else
# define EFLI_UNIQUE_VAR_(name) \
  EFL_TCAT(name, __LINE__, _)
#endif // Counter Check

#endif // EFL_CORE_TRAITS_MACROS_HPP
