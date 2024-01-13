//===- Core/MimAllocator.hpp ----------------------------------------===//
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
//  Fast allocator utilizing the mimalloc library.
//
//===----------------------------------------------------------------===//

#pragma once

#ifndef EFL_CORE_MIMALLOCATOR_HPP
#define EFL_CORE_MIMALLOCATOR_HPP

#include <memory>
#include "Fundamental.hpp"
#include "Traits/Functions.hpp"
#include "_Builtins.hpp"
#include "_Version.hpp"

#if (EFLI_HAS_CXPREVAL_ == 1)
# define EFLI_MI_CXPR_ HINT_INLINE constexpr
#else
# define EFLI_MI_CXPR_ ALWAYS_INLINE
#endif

namespace efl {
namespace C {
#if CPPVER_LEAST(23)
/// Alias for `std::allocation_result`. 
/// Returned from allocate_at_least.
template <typename T, 
  typename SizeType = H::SzType>
using AllocationResult = 
  std::allocation_result<T, SizeType>;
#else
/// Returned from allocate_at_least.
template <typename T, 
  typename SizeType = H::SzType>
struct AllocationResult {
  T ptr;
  SizeType count;
};
#endif

template <typename T, typename SizeType>
FICONSTEXPR auto to_generic_alloc_result(
  AllocationResult<T, SizeType> alloc) NOEXCEPT
 -> AllocationResult<void*, H::SzType> {
  return { static_cast<void*>(alloc.ptr), 
    H::SzType(alloc.count) };
}

template <typename T, typename SizeType = H::SzType>
ALWAYS_INLINE EFLI_CXX20_CXPR_ auto to_typed_alloc_result(
 AllocationResult<void*, H::SzType> alloc) NOEXCEPT
 -> AllocationResult<T, SizeType> {
  return { static_cast<T*>(alloc.ptr), 
    static_cast<SizeType>(alloc.count) };
}

namespace H {
  GLOBAL SzType mimalloc_small_count = 128;

  struct MimAllocatorBase {
    using VoidAllocResult = AllocationResult<void*, SzType>;
    static constexpr SzType smallAllocMax = 
      sizeof(void*) * mimalloc_small_count;
  public:
    constexpr MimAllocatorBase() = default;
    constexpr MimAllocatorBase(const MimAllocatorBase&) = default;
    constexpr MimAllocatorBase(MimAllocatorBase&&) = default;
  protected:
    NODISCARD static void* Allocate(SzType size);
    NODISCARD static void* Allocate(SzType size, void* hint);
    NODISCARD static void* AllocateSmall(SzType size);
    NODISCARD static void* AllocateAligned(SzType align, SzType size);
    NODISCARD static VoidAllocResult AllocateAtLeast(SzType size);
    static void Deallocate(void* p); 
  public:
    static bool IsMallocRedirected();
    static bool IsInHeapRegion(const void* p);
  };

  template <typename T, MEflEnableIf(
    (sizeof(T) <= mimalloc_small_count))>
  FICONSTEXPR bool is_small_alloc(SzType size) NOEXCEPT {
    return EFLI_EXPECT_TRUE_(
      size < MimAllocatorBase::smallAllocMax);
  }

  template <typename T, MEflEnableIf(
    (sizeof(T) > mimalloc_small_count) &&
    (sizeof(T) <= MimAllocatorBase::smallAllocMax))>
  FICONSTEXPR bool is_small_alloc(SzType size) NOEXCEPT {
    return EFLI_EXPECT_FALSE_(
      size < MimAllocatorBase::smallAllocMax);
  }

  template <typename T, MEflEnableIf(
    (sizeof(T) > MimAllocatorBase::smallAllocMax))>
  FICONSTEXPR bool is_small_alloc(SzType size) NOEXCEPT {
    return false;
  }
} // namespace H

template <typename T>
struct MSVC_EMPTY_BASES MimAllocator 
 : H::MimAllocatorBase {
  using value_type = T;
  using propagate_on_container_move_assignment = H::TrueType;

#if CPPVER_MOST(17)
  using pointer = T*;
  using const_pointer = const T*;
  using reference = T&;
  using const_reference = const T&;
  using is_always_equal = H::TrueType;

  template <typename U>
  struct rebind {
    using type = MimAllocator<U>;
  };
#endif // Member Check (C++20)

  using size_type = H::SzType;
  using difference_type = std::ptrdiff_t;

public:
  constexpr MimAllocator() NOEXCEPT = default;
  constexpr MimAllocator(const MimAllocator&) NOEXCEPT = default;
  EFLI_CXX20_CXPR_ ~MimAllocator() = default;

  //=== Member Functions ===//

private:
  NODISCARD EFLI_MI_CXPR_ static void* 
   SmartAllocateImpl(size_type n) NOEXCEPT {
    const auto size = sizeof(T) * n;
    if(H::is_small_alloc<T>(n)) 
      return H::MimAllocatorBase::AllocateSmall(size);
    else 
      return H::MimAllocatorBase::Allocate(size);
  }

  NODISCARD EFLI_MI_CXPR_ static T* 
   SmartAllocate(size_type n) NOEXCEPT {
    return static_cast<T*>(SmartAllocateImpl(n));
  }

public:
#if CPPVER_MOST(17)
  HINT_INLINE pointer address(
   reference r) const NOEXCEPT {
    return X11::addressof(r);
  }

  HINT_INLINE const_pointer address(
   const_reference cr) const NOEXCEPT {
    return X11::addressof(cr);
  }
#endif // C++20 Removal Check

  NODISCARD EFLI_MI_CXPR_ T* allocate(size_type n) {
#if (EFLI_HAS_CXPREVAL_ == 1)
    if(EFL_RT_CXPREVAL()) UNLIKELY {
      return static_cast<T*>(::operator new(n));
    }
#endif
    return SmartAllocate(n);
  }

  EFLI_MI_CXPR_ void deallocate(T* ptr, MAYBE_UNUSED size_type n) {
#if (EFLI_HAS_CXPREVAL_ == 1)
    if(EFL_RT_CXPREVAL()) UNLIKELY {
      ::operator delete(ptr);
      return;
    }
#endif
    // Ensure pointer was allocated by us.
    EFLI_DBGASSERT_(H::MimAllocatorBase::IsInHeapRegion(ptr));
    return H::MimAllocatorBase::Deallocate(ptr);
  }

#if CPPVER_MOST(17)
  size_type max_size() const NOEXCEPT {
    return H::Max<size_type>::value / sizeof(T);
  }

  template <typename U, typename...Args>
  void construct(U* p, Args&&...args) {
    (void) new((void*)p) U(FWD(args)...);
  }

  template <typename U>
  void destroy(U* p) { p->~U(); }
#endif // C++20 Removal Check

  //=== Non-Member Functions ===//

  template <typename T1, typename T2>
  friend constexpr bool operator==(
   const MimAllocator<T1>& t1, const MimAllocator<T2>& t2)
   NOEXCEPT { return true; }

#if CPPVER_MOST(17)
  template <typename T1, typename T2>
  friend constexpr bool operator!=(
   const MimAllocator<T1>& t1, const MimAllocator<T2>& t2)
   NOEXCEPT { return false; }
#endif // Three-way Comparison Check (C++20)
};

} // namespace C
} // namespace efl

#undef EFLI_MI_CXPR_

#endif // EFL_CORE_MIMALLOCATOR_HPP
