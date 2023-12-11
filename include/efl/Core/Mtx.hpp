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
namespace H { using ::std::apply; }
#else
namespace H {
namespace xx11 {
  template <typename F, typename Tup, H::SzType...II>
  struct apply_i_result {
    using type = invoke_result_t<F,
      decltype(std::get<II>(Decl<Tup>()))...>;
  };

  template <typename F, typename Tup, H::SzType...II>
  constexpr auto apply_i(
   F&& f, Tup&& tup, SzSeq<II...>)
   -> typename apply_i_result<F, Tup, II...>::type {
    using ::efl::C::H::invoke;
    return invoke(EFLI_CXPRFWD_(f),
      std::get<II>(cxpr_forward<Tup>(tup))...);
  }

  template <typename F, typename Tup>
  struct apply_result {
    using RTup = MEflGTy(std::decay<Tup>);
    static constexpr H::SzType 
      seqValue = std::tuple_size<RTup>::value;
    using type = decltype(apply_i(
      Decl<F>(), Decl<Tup>(), MkSzSeq<seqValue>{}));
  };

  template <typename F, typename Tup>
  using apply_result_t = MEflGTy(apply_result<F, Tup>);

  template <typename F, typename Tup>
  FICONSTEXPR auto apply(F&& f, Tup&& tup) 
   -> apply_result_t<F, Tup> {
    using Result = apply_result<F, Tup>;
    return apply_i(EFLI_CXPRFWD_(f), 
      EFLI_CXPRFWD_(tup), MkSzSeq<Result::seqValue>{});
  }
} // namespace xx11
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
  explicit ScopedLock(AdoptLock, MTs&...mts)
   : mtxs_(std::tie(mts...)) { }
  
  ~ScopedLock() {

  }

private:
  std::tuple<MTs&...> mtxs_;
};
#endif // std::scoped_lock (C++17)

template <typename...MTs>
inline auto make_scoped_lock(MTs&...mts) 
 -> ScopedLock<MEflGTy(std::decay<MTs>)...> {
  using LockType = ScopedLock<MEflGTy(std::decay<MTs>)...>;
  return LockType { mts... };
}

} // namespace C
} // namespace efl

#endif // EFL_CORE_SCOPEDMTX_HPP
