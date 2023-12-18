//===- Core/Traits/Std.hpp ------------------------------------------===//
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
//  This file implements or includes the standard type_traits.
//
//===----------------------------------------------------------------===//

#pragma once

#ifndef EFL_CORE_TRAITS_STD_HPP
#define EFL_CORE_TRAITS_STD_HPP

#include "Macros.hpp"
#include "StdInt.hpp"

#define USE_(n) using ::std::n;
// For typename ..._t traits.
#define USET_(n) using ::std::n;
// For ..._v traits.
#define USEV_(n) using ::std::n;

//=== Base Objects ===//
namespace efl {
namespace C {
//=== Helpers ===//
namespace H {
  template <typename T>
  using type_ = typename T::type;

  template <bool B, typename T = void>
  using enable_if_ = typename 
    std::enable_if<B, T>::type;
  
  template <bool B>
  using substitute_if = enable_if_<B, bool>;
  
  template <bool B, typename T, typename F>
  using conditional_ = typename 
    std::conditional<B, T, F>::type;
  
  template <template <typename...> class F, typename...Args>
  struct Lazy_ : F<Args...> { };

  namespace logical_ {
    template <typename...>
    using ExpandToTrue = TrueType;

    template <typename...TT>
    ExpandToTrue<enable_if_<TT::value>...> 
     and_helper_(int); // NOLINT
    
    template <typename...>
    FalseType and_helper_(...); // NOLINT

    template <bool>
    struct TOr;

    template <>
    struct TOr<true> {
      template <class, typename Arg, typename...Args>
      using Type_ = typename TOr<!bool(Arg::value) &&
        sizeof...(Args) != 0>::template Type_<Arg, Args...>;
    };

    template <>
    struct TOr<false> {
      template <class R, typename...>
      using Type_ = R;
    };
  } // namespace logical_

  template <typename...TT>
  using And = decltype(logical_::and_helper_<TT...>(0));

  template <typename...TT>
  using Or = typename logical_::TOr<
    sizeof...(TT) != 0>::template Type_<FalseType, TT...>;
  
  template <typename T>
  using Not = H::BoolC<!bool(T::value)>;
} // namespace H

//=== C++11 Traits ===//
#include "Std.cxx11.mac"

//=== C++14 Traits ===//
#if CPPVER_LEAST(14)
USE_(is_null_pointer)
// Unimplementible
USE_(is_final)
#else
template <typename T>
struct is_null_pointer : H::FalseType { };

template <>
struct is_null_pointer<
 decltype(nullptr)> : H::TrueType { };
#endif // C++14 Check

//=== C++17 Traits ===//
#if CPPVER_LEAST(17)
// Swappable
USE_(is_swappable_with)
USE_(is_swappable)
USE_(is_nothrow_swappable_with)
USE_(is_nothrow_swappable)
// Logical
USE_(conjunction)
USE_(disjunction)
USE_(negation)
// Unimplementible
USE_(is_aggregate)
#else
namespace H {
  // TODO: Make sure to use void_t
  template <typename...>
  struct VoidTBase {
    using type = void;
  };

  namespace swap_ {
    using std::swap;

    struct TIsSwappableWith {
      template <typename T, typename U,
        typename = decltype(swap(
          std::declval<T&>(), std::declval<U&>()))>
      static TrueType Test(int); // NOLINT

      template <typename, typename>
      static FalseType Test(...); // NOLINT
    };

    struct TIsNothrowSwappableWith {
      template <typename T, typename U>
      static BoolC<noexcept(swap(
        std::declval<T&>(), std::declval<U&>()))> Test(int); // NOLINT

      template <typename, typename>
      static FalseType Test(...); // NOLINT
    };
  } // namespace swap_

  template <typename T, typename U>
  struct IsSwappableWith {
    using type = decltype(
      swap_::TIsSwappableWith::
      template Test<T, U>(0));
  };

  template <typename T, typename U>
  struct IsNothrowSwappableWith {
    using type = decltype(
      swap_::TIsNothrowSwappableWith::
      template Test<T, U>(0));
  };
} // namespace H

template <typename T, typename U>
struct is_swappable_with 
 : H::IsSwappableWith<T, U>::type { };

template <typename T>
struct is_swappable
 : H::IsSwappableWith<T, T>::type { };

template <typename T, typename U>
struct is_nothrow_swappable_with 
 : H::IsNothrowSwappableWith<T, U>::type { };

template <typename T>
struct is_nothrow_swappable
 : H::IsNothrowSwappableWith<T, T>::type { };

// Logical
template <typename...>
struct conjunction : H::TrueType { };

template <typename Arg, typename...Args>
struct conjunction<Arg, Args...> 
 : H::And<Arg, Args...> { };

template <typename...>
struct disjunction : H::FalseType { };

template <typename Arg, typename...Args>
struct disjunction<Arg, Args...> 
 : H::Or<Arg, Args...> { };

template <typename Arg>
struct negation : H::Not<Arg> { };

# if __has_builtin(__is_aggregate)
template <typename T>
struct is_aggregate 
 : H::BoolC<__is_aggregate(T)> { };
# endif // __has_builtin(__is_aggregate)
#endif // C++17 Check

//=== C++20 Traits ===//
#if CPPVER_LEAST(20)
USE_(is_nothrow_convertible)
USE_(is_bounded_array)
USE_(is_unbounded_array)
// Modifiers
USE_(remove_cvref)
USE_(type_identity)
// Member relationships
# ifdef __cpp_lib_is_pointer_interconvertible
USE_(is_pointer_interconvertible_base_of)
USE_(is_pointer_interconvertible_with_class) // Function
// Unimplementible
USE_(is_layout_compatible)
USE_(is_corresponding_member) // Function
# endif // is_pointer_interconvertible_...
#else
namespace H {
  /// For is_nothrow_convertible
  namespace conv_ {
    template <typename T>
    void is_noexcept_(T) noexcept; // NOLINT

    template <typename T, typename U>
    BoolC<noexcept(
      is_noexcept_<U>(std::declval<T>()))> 
     is_nothrow_convertible_(); // NOLINT
  } // namespace conv_

  template <typename T, typename U>
  struct IsNothrowConvertible : decltype(
    conv_::is_nothrow_convertible_<T, U>()) { };
} // namespace H

template <typename From, typename To>
struct is_nothrow_convertible
 : H::Or<H::And<is_void<From>, is_void<To>>,
  H::Lazy_<H::And, is_convertible<From, To>, 
    H::IsNothrowConvertible<From, To>>>::type { };

template <typename T>
struct is_bounded_array : H::FalseType { };

template <typename T, H::SzType N>
struct is_bounded_array<T[N]> : H::TrueType { };

template <typename T>
struct is_unbounded_array : H::FalseType { };

template <typename T>
struct is_unbounded_array<T[]> : H::TrueType { };

// Modifiers
template <typename T>
struct remove_cvref {
  using type = typename std::remove_reference<
    typename std::remove_cv<T>::type>::type;
};

template <typename T>
struct type_identity { using type = T; };
// TODO: c++20 traits
#endif // C++20 Check

//=== C++23 Traits ===//
#if CPPVER_LEAST(23)
USE_(is_scoped_enum)
// Unimplementible? // TODO
USE_(is_implicit_lifetime)
USE_(reference_constructs_from_temporary)
USE_(reference_converts_from_temporary)
#else
namespace H {
  template <typename T, 
    bool = is_enum<T>::value>
  struct IsScopedEnum : FalseType { };

  template <typename T>
  struct IsScopedEnum<T, true> 
   : BoolC<is_convertible<T, 
    type_<underlying_type<T> >>::value> { };
} // namespace H

template <typename T>
struct is_scoped_enum 
 : H::IsScopedEnum<T> { };
#endif // C++23 Check
} // namespace C
} // namespace efl


#undef USET_
#undef USEV_

#if CPPVER_LEAST(14)
// Exclude traits not matching `<typename T>`.
# define EFLI_EXCLUDE_VEXCEPTIONS_
# define USEV_(n) template <typename T> \
  GLOBAL decltype(n<T>::value) n##_v = n<T>::value;
#endif

// Exclude traits not matching `<typename T>`.
#define EFLI_EXCLUDE_TEXCEPTIONS_
#define USET_(n) template <typename T> \
 using n##_t = typename n<T>::type;

//=== C++11 Traits ===//
namespace efl {
namespace C {
//=== Defaults ===//
#include "Std.cxx11.mac"

//=== Exceptions ===//
// Type Exceptions
template <bool B, typename T = void>
using enable_if_t = typename 
  std::enable_if<B, T>::type;

template <bool B, typename T, typename F>
using conditional_t = typename 
  std::conditional<B, T, F>::type;

template <typename...TT>
using common_type_t = typename
  std::common_type<TT...>::type;

#undef EFLI_EXCLUDE_TEXCEPTIONS_

// Value Exceptions
#if defined(EFLI_EXCLUDE_VEXCEPTIONS_)
// Type checks
template <typename T, typename U>
GLOBAL bool is_same_v =
  std::is_same<T, U>::value;

template <typename From, typename To>
GLOBAL bool is_convertible_v =
  std::is_convertible<From, To>::value;

template <typename Base, typename Derived>
GLOBAL bool is_base_of_v =
  std::is_base_of<Base, Derived>::value;

// Constructible
template <typename T, typename...Args>
GLOBAL bool is_constructible_v =
  std::is_constructible<T, Args...>::value;

template <typename T, typename...Args>
GLOBAL bool is_trivially_constructible_v =
  std::is_trivially_constructible<T, Args...>::value;

template <typename T, typename...Args>
GLOBAL bool is_nothrow_constructible_v =
  std::is_nothrow_constructible<T, Args...>::value;

// Assignable
template <typename T, typename U>
GLOBAL bool is_assignable_v = 
  std::is_assignable<T, U>::value;

template <typename T, typename U>
GLOBAL bool is_trivially_assignable_v = 
  std::is_trivially_assignable<T, U>::value;

template <typename T, typename U>
GLOBAL bool is_nothrow_assignable_v = 
  std::is_nothrow_assignable<T, U>::value;

// Misc.
template <typename T, std::size_t N = 0U>
GLOBAL std::size_t extent_v = std::extent<T, N>::value;
# undef EFLI_EXCLUDE_VEXCEPTIONS_
#endif // Define Excluded ..._v's
} // namespace C
} // namespace efl

//=== C++14 - C++23 Traits ===//
namespace efl {
namespace C {
USET_(remove_cvref)
USET_(type_identity)

USEV_(is_final)
USEV_(is_null_pointer)
USEV_(is_swappable)
USEV_(is_nothrow_swappable)
USEV_(negation)
USEV_(is_bounded_array)
USEV_(is_unbounded_array)
USEV_(is_scoped_enum)

#if CPPVER_LEAST(14)
template <typename T, typename U>
GLOBAL bool is_swappable_with_v = 
  is_swappable_with<T, U>::value;

template <typename T, typename U>
GLOBAL bool is_nothrow_swappable_with_v = 
  is_nothrow_swappable_with<T, U>::value;

template <typename From, typename To>
GLOBAL bool is_nothrow_convertible_v = 
  is_nothrow_convertible<From, To>::value;

template <typename...TT>
GLOBAL bool conjunction_v = 
  conjunction<TT...>::value;

template <typename...TT>
GLOBAL bool disjunction_v = 
  disjunction<TT...>::value;
#endif

#if CPPVER_LEAST(17)
USE_(is_aggregate_v)
#elif __has_builtin(__is_aggregate)
USEV_(is_aggregate)
#endif

// std::void_t<...>
#if CPPVER_LEAST(17)
USE_(void_t)
#else
template <typename...TT>
using void_t = typename 
  H::VoidTBase<TT...>::type;
#endif

#if CPPVER_LEAST(20)
# ifdef __cpp_lib_is_pointer_interconvertible
USE_(is_pointer_interconvertible_base_of_v)
USE_(is_layout_compatible_v)
# endif
#endif

#if CPPVER_LEAST(23)
USE_(is_implicit_lifetime_v)
USE_(reference_constructs_from_temporary_v)
USE_(reference_converts_from_temporary_v)
#endif
} // namespace C
} // namespace efl

#undef USET_
#undef USEV_
#undef USE_

#include <efl/Core/_Fwd/Option.hpp>
#include <efl/Core/_Fwd/Ref.hpp>

//=== Extras ===//
namespace efl {
namespace C {
template <typename T, typename U>
struct is_different : H::TrueType { };

template <typename T>
struct is_different<T, T> : H::FalseType { };

#if CPPVER_LEAST(14)
template <typename T, typename U>
GLOBAL bool is_different_v =
  is_different<T, U>::value;

#endif // C++14 Check
} // namespace C
} // namespace efl

#endif // EFL_CORE_TRAITS_STD_HPP
