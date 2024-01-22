//===- Core/StrRef.hpp ----------------------------------------------===//
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
//  C++11 compatible reimplementation of std::string_view,
//  with some extra utility functions. Based on llvm's implementation.
//
//===----------------------------------------------------------------===//

#pragma once

#ifndef EFL_CORE_STRREF_HPP
#define EFL_CORE_STRREF_HPP

#include <algorithm>
#include <cstring>
#include <iterator>
#if CPPVER_LEAST(17)
# include <string_view>
#endif
#include "Str.hpp"
#include "Traits.hpp"
#include "_Cxx11Assert.hpp"
#include "_Version.hpp"

EFLI_CXPR11ASSERT_PROLOGUE_

#if CPPVER_MOST(14) && \
 __has_builtin(__builtin_strlen)
# define EFLI_CXPR_STRLEN_ constexpr
# define EFLI_STRLEN_(s) __builtin_strlen(s)
#elif CPPVER_LEAST(17)
# define EFLI_CXPR_STRLEN_ constexpr
# define EFLI_STRLEN_(s) \
  std::char_traits<char>::length(s)
#else
# define EFLI_CXPR_STRLEN_
# define EFLI_STRLEN_(s) \
  std::char_traits<char>::length(s)
#endif

// TODO: Finish implementation (consume, parse, search functions)

namespace efl {
namespace C {
/**
 * @brief Non-owning view over a constant string.
 * 
 * Implementation/extension of `std::string_view`.
 * Internals are public, meaning it can be used
 * as a template parameter in C++20.
 */
struct GSL_POINTER StrRef {
  using Type = char;
  using value_type = char;
  using iterator = const char*;
  using const_iterator = const char*;
  using reverse_iterator = std::reverse_iterator<iterator>;
  using size_type = H::SzType;
  using difference_type = std::ptrdiff_t;
  static constexpr size_type npos = ~size_type(0);

private:
  using Traits = std::char_traits<char>;

  ALWAYS_INLINE static int Memcmp(
   const char* l, const char* r, size_type len) NOEXCEPT {
    if(EFL_UNLIKELY(len == 0)) return 0;
    return std::memcmp(l, r, len);
  }

  ALWAYS_INLINE EFLI_CXX20_CXPR_ static bool CxprMemcmp(
   const char* l, const char* r, size_type len) NOEXCEPT {
#if CPPVER_LEAST(20)
    if(EFL_RT_CXPREVAL()) UNLIKELY {
      if(len == 0) return 0;
      return std::equal(l, l + len, r); 
    }
#endif // C++20 Check
    return !!StrRef::Memcmp(l, r, len);
  }

public:
  /// Default constructed.
  constexpr StrRef() NOEXCEPT = default;
  /// Copy constructed.
  constexpr StrRef(const StrRef&) NOEXCEPT = default;

  /// Constructs from a `(const char*, size_type)`.
  constexpr StrRef(const char* str, size_type len) 
   : data_(str), size_(len) { }

  /// Constructs from a `const char*`.
  EFLI_CXPR_STRLEN_ StrRef(const char* str) 
   : data_(str), size_(str ?
   EFLI_STRLEN_(str) : 0) { }
  
  /// Constructs from a `const char(&)[N]`.
  template <H::SzType N>
  constexpr StrRef(carray_t<N>& str) NOEXCEPT
   : data_(str), size_(N) { }
  
  /// Constructs from a `std::string` variant.
  template <typename A>
  StrRef(const BasicStr<char, A>& str)
   : data_(str.data()), size_(str.size()) { }

#if CPPVER_LEAST(17)
  /// Constructs from a `std::string_view`
  constexpr StrRef(std::string_view str) NOEXCEPT 
   : data_(str.data()), size_(str.size()) { }
#endif
  /// Deleted constructor, only use valid data.
  constexpr StrRef(std::nullptr_t) = delete;

  //=== Iterators ===//

  FICONSTEXPR iterator begin() const
  { return data_; }

  FICONSTEXPR iterator end() const
  { return data_ + size_; } 

  EFLI_CXX17_CXPR_ reverse_iterator rbegin() const 
  { return reverse_iterator(begin()); }

  EFLI_CXX17_CXPR_ reverse_iterator rend() const 
  { return reverse_iterator(end()); }

  //=== Element Access ===//

  /// Get character at `n`, returns `const char&`.
  NODISCARD constexpr const char& 
   operator[](size_type n) const& {
    EFLI_CXPR11ASSERT_(n < size_);
    return data_[n];
  }

  /// Get character at `n`, returns `char`.
  NODISCARD constexpr char 
   operator[](size_type n) const&& {
    EFLI_CXPR11ASSERT_(n < size_);
    return data_[n];
  }

  // TODO: at(n) -> panic()?

  /// Get first character, returns `const char&`.
  NODISCARD constexpr const char& front() const& {
    EFLI_CXPR11ASSERT_(!isEmpty());
    return data_[0];
  }

  /// Get first character, returns `char`.
  NODISCARD constexpr char front() const&& {
    EFLI_CXPR11ASSERT_(!isEmpty());
    return data_[0];
  }

  /// Get last character, returns `const char&`.
  NODISCARD constexpr const char& back() const& {
    EFLI_CXPR11ASSERT_(!isEmpty());
    return data_[size_ - 1];
  }

  /// Get last character, returns `char`.
  NODISCARD constexpr char back() const&& {
    EFLI_CXPR11ASSERT_(!isEmpty());
    return data_[size_ - 1];
  }

  //=== Observers ===//

  /// Get a pointer to the beginning of the string.
  EFLI_CXX17_CXPR_ const char* data() const 
  { return data_; }

  /// Get the size of the string.
  FICONSTEXPR size_type size() const 
  { return size_; }

  /// Check if the string is empty (`size() == 0`).
  FICONSTEXPR bool isEmpty() const 
  { return size_ == 0; }

  /// Check if two strings are equal.
  EFLI_CXX20_CXPR_ bool isEqual(StrRef str) const {
    if(size_ != str.size_) return false;
    return StrRef::CxprMemcmp(
      begin(), str.begin(), size_);
  }

  //=== Modifiers ===//

  /// Remove `n` characters from the start of the string.
  EFLI_CXX14_CXPR_ void removePrefix(size_type n) {
    EFLI_DBGASSERT_(data_ && n <= size_);
    this->data_ += n;
    this->size_ -= n;
  }

  /// Remove `n` characters from the end of the string.
  EFLI_CXX14_CXPR_ void removeSuffix(size_type n) {
    EFLI_DBGASSERT_(n <= size_);
    this->size_ -= n;
  }

  /// Remove `sizeof(s) - 1` characters from the start of the string.
  /// Useful for things like `sv.removePrefixWith("abc")`.
  template <H::SzType N>
  EFLI_CXX14_CXPR_ void removePrefixWith(carray_t<N>&) {
    EFLI_DBGASSERT_(data_ && (N - 1) <= size_);
    this->data_ += (N - 1);
    this->size_ -= (N - 1);
  }

  /// Remove `sizeof(s) - 1` characters from the end of the string.
  /// Useful for things like `sv.removeSuffixWith("xyz")`.
  template <H::SzType N>
  EFLI_CXX14_CXPR_ void removeSuffixWith(carray_t<N>&) {
    EFLI_DBGASSERT_((N - 1) <= size_);
    this->size_ -= (N - 1);
  }

  /// Swaps `s` with `*this`.
  EFLI_CXX14_CXPR_ void swap(StrRef& s) noexcept {
    StrRef tmp = *this;
    *this = s;
    s = tmp;
  }

  //=== Operations ===//

  EFLI_CXX20_CXPR_ size_type copy(Type* dst, 
   size_type count, size_type pos = 0) const {
    EFLI_CXPRASSERT_(pos <= size_);
    size_type rcount = count > (size_ - pos) ? 
      (size_ - pos) : count;
    Type* odst = Traits::copy(
      dst, (data_ + count), rcount);
    return static_cast<size_type>(odst - dst);
  }

  /// Return a `StrRef` starting at `pos` with `n` characters.
  FICONSTEXPR StrRef slice(size_type pos, size_type n) const {
    EFLI_CXPR11ASSERT_(data_ && (pos + n) <= size_);
    return StrRef(data_ + pos, n);
  }

  /// Return a `StrRef` with `n` characters removed.
  FICONSTEXPR StrRef slice(size_type n) const {
    EFLI_CXPR11ASSERT_(data_ && n <= size_);
    return StrRef(data_ + n, size_ - n);
  }

  /// Return a `StrRef` with `n` characters removed
  /// from the start of the string.
  constexpr StrRef dropFront(size_type n = 1) const 
  { return slice(n); }

  /// Return a `StrRef` with `n` characters removed
  /// from the end of the string.
  constexpr StrRef dropBack(size_type n = 1) const {
    EFLI_CXPR11ASSERT_(n <= size_);
    return slice(0, size_ - n);
  }

  /// Return a `StrRef` with `n` characters
  /// from the start of the string.
  constexpr StrRef takeFront(size_type n = 1) const {
    if(n >= size_) return *this;
    return dropBack(size_ - n);
  }

  /// Return a `StrRef` with `n` characters
  /// from the end of the string.
  constexpr StrRef takeBack(size_type n = 1) const {
    if(n >= size_) return *this;
    return dropFront(size_ - n);
  }

  // find, findSlow (cxpr) [and associated functions]

  /// Copy contents of `StrRef` to a new `Str`.
  HINT_INLINE Str toStr() const { 
    return Str(data(), size()); 
  }

  /// Copy contents of `StrRef` to a new `OveralignedStr<N>`.
  template <H::SzType N> 
  HINT_INLINE OveralignedStr<N>
   toOveralignedStr() const {
    return OveralignedStr<N>(data(), size());
  }
  
  /// Implicitly create new `Str` from `StrRef`.
  operator Str() const { return this->toStr(); }

public:
  const char* data_ = nullptr;
  size_type   size_ = 0;
};

} // namespace C
} // namespace efl

#undef EFLI_CXPR_STRLEN_
#undef EFLI_STRLEN_

EFLI_CXPR11ASSERT_EPILOGUE_

#endif // EFL_CORE_STRREF_HPP
