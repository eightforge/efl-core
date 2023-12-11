//===- Core/Mtx.hpp -------------------------------------------------===//
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
//  This file is used as a placeholder for a future mutex 
//  implementation. It also implements std::scoped_lock pre-C++17.
//
//===----------------------------------------------------------------===//

#pragma once

#ifndef EFL_CORE_SCOPEDMTX_HPP
#define EFL_CORE_SCOPEDMTX_HPP

#include <efl/Core/Fwd_/Mtx.hpp>
#include "Traits.hpp"
#if CPPVER_MOST(14)
# include <tuple>
#endif

namespace efl {
namespace C {
namespace H {
  using DirectMutex = std::mutex;
  enum class MtxNullHandle : i64 { HNull = 0 };

  template <typename T, typename = void>
  struct TMtxHasNativeHandle : FalseType {
    using HandleType = MtxNullHandle;
  };

  template <typename T>
  struct TMtxHasNativeHandle<T, 
    decltype(void(Decl<T&>().native_handle()))>
   : TrueType {
    using HandleType = 
      typename T::native_handle_type;
  };

  /// Checks the default mutex, `std::mutex`.
  using MtxHasNativeHandle =
    TMtxHasNativeHandle<DirectMutex>;
  /// The native handle type, or `MtxNullHandle`.
  using MtxNativeHandle = 
    typename MtxHasNativeHandle::HandleType;

  /// Wrapper around `DirectMutex`.
  template <bool HasNativeHandle>
  struct Mtx : DirectMutex {
    ALWAYS_INLINE bool tryLock() {
      return DirectMutex::try_lock();
    }

    ALWAYS_INLINE auto nativeHandle() 
     -> MtxNativeHandle {
      return DirectMutex::native_handle();
    }
  };

  /// Wrapper around `DirectMutex`.
  /// `native_handle()` does NOT exist.
  template <>
  struct Mtx<false> : DirectMutex {
    ALWAYS_INLINE bool tryLock() {
      return DirectMutex::try_lock();
    }

    ALWAYS_INLINE auto nativeHandle() 
     -> MtxNullHandle {
      return MtxNullHandle::HNull;
    }
  };
} // namespace H

using Mtx = H::Mtx<
  H::MtxHasNativeHandle::value>;

#ifdef __cpp_lib_scoped_lock
template <typename...TT>
using ScopedLock = ::std::scoped_lock<TT...>;
#else
namespace H {
  struct ScopedUnlocker {
  private:
    template <typename MT>
    ALWAYS_INLINE ibyte ID(MT& mt) {
      mt.unlock();
      return ibyte(0);
    }

    template <typename...CC>
    ALWAYS_INLINE void Ignore(CC...) { }

  public:
    template <typename...MTs>
    ALWAYS_INLINE void operator()(MTs&...mts) CONST {
      (void)(ScopedUnlocker::Ignore(
        ScopedUnlocker::ID(mts)...));
    }
  };

  GLOBAL ScopedUnlocker scoped_unlock { };
} // namespace H

/**
 * A lock for multiple `Mtx` objects. It holds
 * the locks while in scope, and releases them
 * when it goes out of scope.
 */
template <typename...MTs>
struct ScopedLock {
  /// Acquires ownership.
  explicit ScopedLock(MTs&...mts)
   : mtxs_(std::tie(mts...)) {
    std::lock(mts...);
  }

  /// Non-owning management.
  explicit ScopedLock(AdoptLock, MTs&...mts) NOEXCEPT
   : mtxs_(std::tie(mts...)) { }
  
  ScopedLock(const ScopedLock&) = delete;
  ScopedLock& operator=(const ScopedLock&) = delete;
  ~ScopedLock() { H::apply(H::scoped_unlock, mtxs_); }

private:
  std::tuple<MTs&...> mtxs_;
};

/// Single item lock. Avoids tuples.
template <typename MT>
struct ScopedLock<MT> {
  using mutex_type = MT;
  explicit ScopedLock(MT& mt) : mtx_(mt) { mt.lock(); }
  explicit ScopedLock(AdoptLock, MT& mt) NOEXCEPT : mtx_(mt) { }

  ScopedLock(const ScopedLock&) = delete;
  ScopedLock& operator=(const ScopedLock&) = delete;
  ~ScopedLock() { mtx_.unlock(); }

private:
  MT& mtx_;
};

/// Empty lock, noop.
template <>
struct ScopedLock<> {
  explicit ScopedLock() = default;
  explicit ScopedLock(AdoptLock) NOEXCEPT { }
  ScopedLock(const ScopedLock&) = delete;
  ScopedLock& operator=(const ScopedLock&) = delete;
  ~ScopedLock() = default;
};
#endif // std::scoped_lock (C++17)

template <typename...MTs>
using SScopedLock = ScopedLock<MEflGTy(std::decay<MTs>)...>;

template <typename...MTs>
SScopedLock<MTs...> make_scoped_lock(MTs&...mts) {
  return SScopedLock<MTs...>(mts...);
}

} // namespace C
} // namespace efl

#endif // EFL_CORE_SCOPEDMTX_HPP