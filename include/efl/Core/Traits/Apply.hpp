//===- Core/Traits/Apply.hpp ----------------------------------------===//
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

#ifndef EFL_CORE_TRAITS_APPLY_HPP
#define EFL_CORE_TRAITS_APPLY_HPP

#include <tuple>
#include "Invoke.hpp"

namespace efl {
namespace C {
namespace H {
#ifdef __cpp_lib_apply
using ::std::apply;
#else
namespace xx11 {
  template <typename F, typename Tup, H::SzType...II>
  struct apply_i_result {
    using type = invoke_result_t<F,
      decltype(std::get<II>(Decl<Tup>()))...>;
  };

  template <typename F, 
    typename Tup, H::SzType...II>
  using apply_i_result_t = typename
    apply_i_result<F, Tup, II...>::type;

  template <typename F, typename Tup, H::SzType...II>
  constexpr auto apply_i(
   F&& f, Tup&& tup, SzSeq<II...>)
   -> apply_i_result_t<F, Tup, II...> {
    return invoke(cxpr_forward<F>(f),
      std::get<II>(cxpr_forward<Tup>(tup))...);
  }

  template <typename F, typename Tup>
  struct apply_result {
    using RTup = decay_t<Tup>;
    static constexpr H::SzType 
      seqValue = std::tuple_size<RTup>::value;
    using type = decltype(apply_i(
      Decl<F>(), Decl<Tup>(), MkSzSeq<seqValue>{}));
  };

  template <typename F, typename Tup>
  using apply_result_t = typename
    apply_result<F, Tup>::type;

  template <typename F, typename Tup>
  FICONSTEXPR auto apply(F&& f, Tup&& tup) 
   -> apply_result_t<F, Tup> {
    using Result = apply_result<F, Tup>;
    return apply_i(cxpr_forward<F>(f), 
      cxpr_forward<Tup>(tup), MkSzSeq<Result::seqValue>{});
  }
} // namespace xx11

using xx11::apply;
using xx11::apply_result_t;

#endif // Feature check (C++17)
} // namespace H
} // namespace C
} // namespace efl

#endif // EFL_CORE_TRAITS_APPLY_HPP
