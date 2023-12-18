//===- Panic/Handler.cpp --------------------------------------------===//
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

#include <cstdio>
#include <Core/Preload.hpp>
#include <Core/Panic.hpp>
#include "Guard.hpp"

// TODO: Add static initializer, Box<...>, terminate_handler,
// unwinder? (internal exception type), panic types, etc.
// Use `Box<void(PanicInfo&)>` -> `PanicBox`,
// or `DetBox<void(PanicInfo&)>` -> `PanicBox`?

#define PANIC_MSG_(fmt, ...) \
  ::std::fprintf(stderr, fmt, ##__VA_ARGS__)
#define PANIC_ERR_(fmt, ...) \
  PANIC_MSG_("[ERROR] " fmt, ##__VA_ARGS__)

using namespace efl;

NORETURN static void std_thandler_();
NORETURN static void panic_thandler_();

namespace HH {
struct THandlerInit {
  EFL_COLD_PATH C::THandler 
   operator()() const NOEXCEPT {
    // The default terminate callback
    // should exist by this point.
    return std::get_terminate();
  }
};

struct PanicHandlerSetter {
  EFL_COLD_PATH void operator()() const NOEXCEPT {
    std::set_terminate(&panic_thandler_);
  }
};

// Caches the default terminate_handler on startup.
static const C::Preload<
  C::THandler, THandlerInit> default_thandle_ { };

static const C::StaticExec<
  PanicHandlerSetter> phandler_setter_ { };
} // namespace HH

//=== Underlying Panic Implementation ===//
namespace {
  struct PanicInstance {
    virtual ~PanicInstance() = default;
  };
} // namespace `anonymous`

void efl::C::panic_() {
  throw PanicInstance { };
}

/// Invokes the standard terminate handler.
EFL_COLD_PATH NORETURN void std_thandler_() {
  C::THandler thandler = HH::default_thandle_();
  if(EFLI_EXPECT_TRUE_(thandler)) {
    try {
      thandler();
      PANIC_ERR_("`terminate_handler` "
        "unexpectedly returned.\n");
      ::abort();
    } catch(...) {
      PANIC_ERR_("`terminate_handler` "
        "unexpectedly threw an exception.\n");
      ::abort();
    }
  }
  PANIC_ERR_("`terminate_handler` is NULL.\n");
  EFLI_QABORT_();
}

/// Custom terminate handler.
EFL_COLD_PATH NORETURN void panic_thandler_() {
  EFLI_PANIC_LOCK_();
  C::ExPtr e = std::current_exception();
  // Non-exception termination.
  if(!e) std_thandler_();
  try { std::rethrow_exception(e); }
  // True panic, custom handling.
  catch(const PanicInstance& p) {
    // TODO: Real stuff here!!
    PANIC_MSG_("Boom boom pow "
      "these bitches jackin my style!!\n");
    ::abort();
  }
  // Normal uncaught exceptions, standard handling.
  catch(const std::exception&) { std_thandler_(); } 
  catch(...) { EFLI_QABORT_(); }
  EFL_UNREACHABLE();
}

