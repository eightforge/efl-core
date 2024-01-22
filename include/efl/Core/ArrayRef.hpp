//===- Core/ArrayRef.hpp --------------------------------------------===//
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
//  This file defines a generic interface for viewing contiguous data.
//  Based on llvm::ArrayRef<...> and zig slices.
//
//===----------------------------------------------------------------===//

#pragma once

#ifndef EFL_CORE_ARRAYREF_HPP
#define EFL_CORE_ARRAYREF_HPP

#include <algorithm>
#include <iterator>
#include "Array.hpp"
#include "Fundamental.hpp"
#include "Vec.hpp"
#include "Traits/Functions.hpp"
#include "_Fwd/ArrayRef.hpp"
#include "_Fwd/Option.hpp"
#include "_Cxx11Assert.hpp"
#include "_Version.hpp"

#if EFLI_GCCVER_LEAST_(9)
# define EFLI_LIFETIME_PROLOGUE_ GNU_IGNORED("-Winit-list-lifetime")
# define EFLI_LIFETIME_EPILOGUE_ GNU_POP()
#else
# define EFLI_LIFETIME_PROLOGUE_
# define EFLI_LIFETIME_EPILOGUE_
#endif

#ifdef __cpp_deduction_guides
# define EFLI_ARRAYREF_DEPR_(msg) FDEPRECATED(msg)
#else
# define EFLI_ARRAYREF_DEPR_(msg)
#endif

EFLI_CXPR11ASSERT_PROLOGUE_

namespace efl {
namespace C {
/**
 * @name ArrayRef
 * @brief Non-owning view over a contiguous buffer.
 * 
 * Internals are public, meaning it can be used
 * as a template parameter in C++20.
 */
template <typename T>
struct GSL_POINTER NODISCARD ArrayRef {
  using Type = T;
  using SelfType = ArrayRef<T>;
  using RawType_ = remove_const_t<T>;
  using value_type = T;
  using iterator = T*;
  using const_iterator = const T*;
  using reverse_iterator = std::reverse_iterator<iterator>;
  using size_type = H::SzType;
  using difference_type = std::ptrdiff_t;
public:
  /// Default constructed.
  constexpr ArrayRef() = default;
  /// Constructed from nullopt.
  constexpr ArrayRef(NullOpt) { }

  /// Construct from single element.
  EFLI_CXX17_CXPR_ ArrayRef(T& elem)
   : data_(X11::addressof(elem)), size_(1) { }
  
  /// Construct from pointer/length pair.
  constexpr ArrayRef(T* data, size_type size)
   : data_(data), size_(size) { }
  
  /// Construct from pointer/sentinel pair.
  constexpr ArrayRef(T* begin, T* end) 
   : data_(begin), size_(end - begin) {
    EFLI_CXPR11ASSERT_(end >= begin);
  }

  // TODO: SmallVec

  template <typename Alloc>
  EFLI_CXX20_CXPR_ ArrayRef(std::vector<T, Alloc>& vec)
   : data_(vec.data()), size_(vec.size()) { }
  
  template <H::SzType N>
  constexpr ArrayRef(Array<T, N>& arr)
   : data_(arr.data()), size_(N) { }
  
  template <H::SzType N>
  constexpr ArrayRef(array_t<T, N>& raw_arr)
   : data_(raw_arr), size_(N) { }

  template <typename U>
  enable_if_t<is_same<T, U>::value, SelfType>&
   operator=(U&&) = delete;
  
  template <typename U>
  enable_if_t<is_same<T, U>::value, SelfType>&
   operator=(H::InitList<U>) = delete;

  //=== Iterators ===//

  EFLI_CXX14_CXPR_ iterator begin()
  { return data_; }

  EFLI_CXX14_CXPR_ iterator end()
  { return data_ + size_; }

  FICONSTEXPR const_iterator begin() const
  { return data_; }

  FICONSTEXPR const_iterator end() const
  { return data_ + size_; }

  FICONSTEXPR const_iterator cbegin() const
  { return data_; }

  FICONSTEXPR const_iterator cend() const
  { return data_ + size_; }

  EFLI_CXX17_CXPR_ reverse_iterator rbegin() 
  { return reverse_iterator(begin()); }

  EFLI_CXX17_CXPR_ reverse_iterator rend() 
  { return reverse_iterator(end()); }

  //=== Element Access ===//

  /// Get element at `n`, returns `T&`.
  NODISCARD EFLI_CXX14_CXPR_ T& 
   operator[](size_type n) {
    EFLI_CXPR11ASSERT_(n < size_);
    return data_[n];
  }

  /// Get element at `n`, returns `const T&`.
  NODISCARD constexpr const T& 
   operator[](size_type n) const {
    EFLI_CXPR11ASSERT_(n < size_);
    return data_[n];
  }

  /// Get first element, returns `T&`.
  NODISCARD EFLI_CXX14_CXPR_ T& front() {
    EFLI_CXPR11ASSERT_(!isEmpty());
    return data_[0];
  }

  /// Get first element, returns `const T&`.
  NODISCARD constexpr const T& front() const {
    EFLI_CXPR11ASSERT_(!isEmpty());
    return data_[0];
  }

  /// Get last element, returns `T&`.
  NODISCARD EFLI_CXX14_CXPR_ T& back() {
    EFLI_CXPR11ASSERT_(!isEmpty());
    return data_[size_ - 1];
  }

  /// Get last element, returns `const T&`.
  NODISCARD constexpr const T& back() const {
    EFLI_CXPR11ASSERT_(!isEmpty());
    return data_[size_ - 1];
  }

  //=== Observers ===//

  /// Get a pointer to the beginning of the span.
  EFLI_CXX17_CXPR_ const T* data() const 
  { return data_; }

  /// Get the size of the span.
  FICONSTEXPR size_type size() const 
  { return size_; }

  /// Get the size of the span in bytes.
  FICONSTEXPR size_type sizeInBytes() const 
  { return size_ * sizeof(T); }

  /// Check if the span is empty (`size() == 0`).
  FICONSTEXPR bool isEmpty() const 
  { return size_ == 0; }

  EFLI_CXX20_CXPR_ bool isEqual(ArrayRef<T> arr) const {
    if(this->size_ != arr.size_) return false;
    return std::equal(begin(), end(), arr.begin());
  }

  //=== Operations ===//

  /// Return a `ArrayRef<T>` starting at `pos` with `n` elements.
  FICONSTEXPR SelfType slice(size_type pos, size_type n) const {
    EFLI_CXPR11ASSERT_(data_ && (pos + n) <= size_);
    return SelfType(data_ + pos, n);
  }

  /// Return a `ArrayRef<T>` with `n` elements removed.
  FICONSTEXPR SelfType slice(size_type n) const {
    EFLI_CXPR11ASSERT_(data_ && n <= size_);
    return SelfType(data_ + n, size_ - n);
  }

  /// Return a `ArrayRef<T>` with `n` elements removed
  /// from the start of the span.
  constexpr SelfType dropFront(size_type n = 1) const 
  { return slice(n); }

  /// Return a `ArrayRef<T>` with `n` elements removed
  /// from the end of the span.
  constexpr SelfType dropBack(size_type n = 1) const {
    EFLI_CXPR11ASSERT_(n <= size_);
    return slice(0, size_ - n);
  }

  /// Return a `ArrayRef<T>` with `n` elements
  /// from the start of the span.
  constexpr SelfType takeFront(size_type n = 1) const {
    if(n >= size_) return *this;
    return dropBack(size_ - n);
  }

  /// Return a `ArrayRef<T>` with `n` elements
  /// from the end of the span.
  constexpr SelfType takeBack(size_type n = 1) const {
    if(n >= size_) return *this;
    return dropFront(size_ - n);
  }

  /// Copy contents of `ArrayRef<T>` to a new `Vec<T>`.
  HINT_INLINE Vec<RawType_> toVec() const { 
    return Vec<RawType_>(data(), data() + size()); 
  }

  /// Copy contents of `ArrayRef<T>` to a new `OveralignedVec<N>`.
  template <H::SzType N> 
  HINT_INLINE OveralignedVec<RawType_, N> 
   toOveralignedVec() const {
    return OveralignedVec<RawType_, N>(
      data(), data() + size());
  }

  /// Implicitly create new `Vec<T>` from `ArrayRef<T>`.
  operator Vec<RawType_>() const { return this->toVec(); }

public:
  T* data_ = nullptr;
  size_type size_ = 0;
};

template <typename T>
struct NODISCARD ImmutArrayRef : public ArrayRef<const T> {
  using Type = T;
  using SelfType = ImmutArrayRef<T>;
  using BaseType = ArrayRef<const T>;
  using value_type = T;
  using iterator = const T*;
  using const_iterator = const T*;
  using reverse_iterator = std::reverse_iterator<iterator>;
  using size_type = H::SzType;
  using difference_type = std::ptrdiff_t;
public:
  /// Default constructed.
  constexpr ImmutArrayRef() = default;
  /// Constructed from nullopt.
  constexpr ImmutArrayRef(NullOpt) 
   : ArrayRef<const T>() { }

  /// Construct from single element.
  EFLI_CXX17_CXPR_ ImmutArrayRef(const T& elem)
   : ArrayRef<const T>(elem) { }
  
  /// Construct from pointer/length pair.
  constexpr ImmutArrayRef(const T* data, size_type size)
   : ArrayRef<const T>(data, size) { }
  
  /// Construct from pointer/sentinel pair.
  constexpr ImmutArrayRef(const T* begin, const T* end) 
   : ArrayRef<const T>(begin, end) { }

  // TODO: SmallVec

  template <typename Alloc>
  EFLI_CXX20_CXPR_ ImmutArrayRef(
    const std::vector<T, Alloc>& vec)
   : ArrayRef<const T>(vec.data(), vec.size()) { }
  
  template <H::SzType N>
  constexpr ImmutArrayRef(const Array<T, N>& arr)
   : ArrayRef<const T>(arr.data(), N) { }
  
  template <H::SzType N>
  constexpr ImmutArrayRef(
    const array_t<T, N>& raw_arr)
   : ArrayRef<const T>(raw_arr, N) { }

 EFLI_LIFETIME_PROLOGUE_
  constexpr ImmutArrayRef(const H::InitList<T>& il)
   : ArrayRef<const T>((il.begin() == il.end()) ? 
    (T*)nullptr : il.data(), il.size()) { }
 EFLI_LIFETIME_EPILOGUE_

  //=== Iterators ===//

  FICONSTEXPR const_iterator begin() const
  { return BaseType::begin(); }

  FICONSTEXPR const_iterator end() const
  { return BaseType::end(); }

  EFLI_CXX17_CXPR_ reverse_iterator rbegin() const
  { return reverse_iterator(begin()); }

  EFLI_CXX17_CXPR_ reverse_iterator rend() const
  { return reverse_iterator(end()); }

  //=== Element Access ===//

  /// Get element at `n`, returns `const T&`.
  NODISCARD constexpr const T& 
   operator[](size_type n) const {
    EFLI_CXPR11ASSERT_(n < size());
    return data()[n];
  }

  /// Get first element, returns `const T&`.
  NODISCARD constexpr const T& front() const {
    EFLI_CXPR11ASSERT_(!isEmpty());
    return BaseType::front();
  }

  /// Get last element, returns `const T&`.
  NODISCARD constexpr const T& back() const {
    EFLI_CXPR11ASSERT_(!isEmpty());
    return BaseType::back();
  }

  //=== Observers ===//

  /// Get a pointer to the beginning of the span.
  EFLI_CXX17_CXPR_ const T* data() const 
  { return BaseType::data(); }

  /// Get the size of the span.
  FICONSTEXPR size_type size() const 
  { return BaseType::size_; }

  /// Get the size of the span in bytes.
  FICONSTEXPR size_type sizeInBytes() const 
  { return BaseType::size_ * sizeof(T); }

  /// Check if the span is empty (`size() == 0`).
  FICONSTEXPR bool isEmpty() const 
  { return BaseType::size_ == 0; }

  EFLI_CXX20_CXPR_ bool isEqual(ArrayRef<T> arr) const {
    return BaseType::isEqual(arr);
  }

  EFLI_CXX20_CXPR_ bool isEqual(ImmutArrayRef<T> arr) const {
    if(this->size() != arr.size()) return false;
    return std::equal(begin(), end(), arr.begin());
  }

  //=== Operations ===//

  /// Return a `ImmutArrayRef<T>` starting at `pos` with `n` elements.
  FICONSTEXPR SelfType slice(size_type pos, size_type n) const {
    EFLI_CXPR11ASSERT_(data() && (pos + n) <= size());
    return SelfType(data() + pos, n);
  }

  /// Return a `ImmutArrayRef<T>` with `n` elements removed.
  FICONSTEXPR SelfType slice(size_type n) const {
    EFLI_CXPR11ASSERT_(data() && n <= size());
    return SelfType(data() + n, size() - n);
  }

  /// Return a `ImmutArrayRef<T>` with `n` elements removed
  /// from the start of the span.
  constexpr SelfType dropFront(size_type n = 1) const 
  { return slice(n); }

  /// Return a `ImmutArrayRef<T>` with `n` elements removed
  /// from the end of the span.
  constexpr SelfType dropBack(size_type n = 1) const {
    EFLI_CXPR11ASSERT_(n <= size());
    return slice(0, size() - n);
  }

  /// Return a `ImmutArrayRef<T>` with `n` elements
  /// from the start of the span.
  constexpr SelfType takeFront(size_type n = 1) const {
    if(n >= size()) return *this;
    return dropBack(size() - n);
  }

  /// Return a `ImmutArrayRef<T>` with `n` elements
  /// from the end of the span.
  constexpr SelfType takeBack(size_type n = 1) const {
    if(n >= size()) return *this;
    return dropFront(size() - n);
  }
};

//=== Deduction Guides - ArrayRef ===//

template <typename T>
NODEBUG EFLI_CXX17_CXPR_ ArrayRef<T>
 make_arrayref(T& t) NOEXCEPT {
  return ArrayRef<T>(t);
}

template <typename T>
NODEBUG constexpr ArrayRef<T>
 make_arrayref(T* data, H::SzType size) NOEXCEPT {
  return ArrayRef<T>(data, size);
}

template <typename T>
NODEBUG constexpr ArrayRef<T>
 make_arrayref(T* begin, T* end) NOEXCEPT {
  return ArrayRef<T>(begin, end);
}

// SmallVec

template <typename T, typename A>
NODEBUG EFLI_CXX20_CXPR_ ArrayRef<T>
 make_arrayref(std::vector<T, A>& vec) NOEXCEPT {
  return ArrayRef<T>(vec);
}

template <typename T, H::SzType N>
NODEBUG constexpr ArrayRef<T>
 make_arrayref(Array<T, N>& arr) NOEXCEPT {
  return ArrayRef<T>(arr);
}

template <typename T, H::SzType N>
NODEBUG constexpr ArrayRef<T>
 make_arrayref(array_t<T, N>& raw_arr) NOEXCEPT {
  return ArrayRef<T>(raw_arr);
}

template <typename T>
NODEBUG constexpr ArrayRef<T>&
 make_arrayref(ArrayRef<T>& self) NOEXCEPT {
  return self;
}

#ifdef __cpp_deduction_guides
template <typename T>
ArrayRef(T&) -> ArrayRef<T>;

template <typename T>
ArrayRef(T*, H::SzType) -> ArrayRef<T>;

template <typename T>
ArrayRef(T* begin, T* end) -> ArrayRef<T>;

// SmallVec

template <typename T, typename A>
ArrayRef(std::vector<T, A>&) -> ArrayRef<T>;

template <typename T, H::SzType N>
ArrayRef(Array<T, N>&) -> ArrayRef<T>;

template <typename T, H::SzType N>
ArrayRef(array_t<T, N>&) -> ArrayRef<T>;
#endif

//=== Deduction Guides - ImmutArrayRef ===//

template <typename T>
NODEBUG EFLI_CXX17_CXPR_ ImmutArrayRef<T>
 make_immutarrayref(const T& t) NOEXCEPT {
  return ImmutArrayRef<T>(t);
}

template <typename T>
NODEBUG constexpr ImmutArrayRef<T>
 make_immutarrayref(const T* data, H::SzType size) NOEXCEPT {
  return ImmutArrayRef<T>(data, size);
}

template <typename T>
NODEBUG constexpr ImmutArrayRef<T>
 make_immutarrayref(const T* begin, const T* end) NOEXCEPT {
  return ImmutArrayRef<T>(begin, end);
}

// SmallVec

template <typename T, typename A>
NODEBUG EFLI_CXX20_CXPR_ ImmutArrayRef<T>
 make_immutarrayref(const std::vector<T, A>& vec) NOEXCEPT {
  return ImmutArrayRef<T>(vec);
}

template <typename T, H::SzType N>
NODEBUG constexpr ImmutArrayRef<T>
 make_immutarrayref(const Array<T, N>& arr) NOEXCEPT {
  return ImmutArrayRef<T>(arr);
}

template <typename T, H::SzType N>
NODEBUG constexpr ImmutArrayRef<T>
 make_immutarrayref(const array_t<T, N>& raw_arr) NOEXCEPT {
  return ImmutArrayRef<T>(raw_arr);
}

template <typename T>
NODEBUG constexpr ImmutArrayRef<T>
 make_immutarrayref(const H::InitList<T>& il) NOEXCEPT {
  return ImmutArrayRef<T>(il);
}

template <typename T>
NODEBUG constexpr const ImmutArrayRef<T>&
 make_immutarrayref(const ImmutArrayRef<T>& self) NOEXCEPT {
  return self;
}

#ifdef __cpp_deduction_guides
template <typename T>
ImmutArrayRef(const T&) -> ImmutArrayRef<T>;

template <typename T>
ImmutArrayRef(const T*, H::SzType) -> ImmutArrayRef<T>;

template <typename T>
ImmutArrayRef(const T* begin, const T* end) -> ImmutArrayRef<T>;

// SmallVec

template <typename T, typename A>
ImmutArrayRef(const std::vector<T, A>&) -> ImmutArrayRef<T>;

template <typename T, H::SzType N>
ImmutArrayRef(const Array<T, N>&) -> ImmutArrayRef<T>;

template <typename T, H::SzType N>
ImmutArrayRef(const array_t<T, N>&) -> ImmutArrayRef<T>;

template <typename T>
ImmutArrayRef(const H::InitList<T>&) -> ImmutArrayRef<T>;
#endif

//=== Comparisons ===//

namespace ops_ {
  template <typename T, typename U>
  NODEBUG EFLI_CXX20_CXPR_ bool operator==(
   ArrayRef<T> lhs, U&& u) NOEXCEPT {
    const ArrayRef<T> rhs(FWD_CAST(u));
    return lhs.isEqual(rhs);
  }

  template <typename T, typename U>
  NODEBUG EFLI_CXX20_CXPR_ bool operator==(
   ImmutArrayRef<T> lhs, U&& u) NOEXCEPT {
    const ImmutArrayRef<T> rhs(FWD_CAST(u));
    return lhs.isEqual(rhs);
  }

  template <typename T, typename U>
  NODEBUG EFLI_CXX20_CXPR_ bool operator!=(
   ArrayRef<T> lhs, U&& u) NOEXCEPT {
    const ArrayRef<T> rhs(FWD_CAST(u));
    return !lhs.isEqual(rhs);
  }

  template <typename T, typename U>
  NODEBUG EFLI_CXX20_CXPR_ bool operator!=(
   ImmutArrayRef<T> lhs, U&& u) NOEXCEPT {
    const ImmutArrayRef<T> rhs(FWD_CAST(u));
    return !lhs.isEqual(rhs);
  }
} // namespace ops_

} // namespace C
} // namespace efl

EFLI_CXPR11ASSERT_EPILOGUE_

#undef EFLI_ARRAYREF_DEPR_
#undef EFLI_LIFETIME_EPILOGUE_
#undef EFLI_LIFETIME_PROLOGUE_

using namespace efl::C::ops_;

#endif // EFL_CORE_ARRAYREF_HPP
