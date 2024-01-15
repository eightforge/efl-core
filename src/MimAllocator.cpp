//===- MimAllocator.cpp ---------------------------------------------===//
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

#define EFLI_MIMALLOCATOR_INTERNAL_ 1
#include <CoreCommon/Mimalloc.hpp>
#include <Core/MimAllocator.hpp>
#include <mimalloc/types.h>

#define MIMALLOC_BASE efl::C::H::MimAllocatorBase

using namespace efl;
using namespace efl::C;

static_assert(MIMALLOC_BASE::smallAllocMax <= MI_SMALL_SIZE_MAX,
  "Incorrectly estimated the max size. Let us know about this.");

void* MIMALLOC_BASE::Allocate(H::SzType size) {
  return mi_malloc(size);
}

void* MIMALLOC_BASE::Allocate(H::SzType size, void* hint) {
  (void)hint;
  return mi_malloc(size);
}

void* MIMALLOC_BASE::AllocateSmall(H::SzType size) {
  $raw_assert(size <= MIMALLOC_BASE::smallAllocMax);
  return mi_malloc_small(size);
}

void* MIMALLOC_BASE::AllocateAligned(H::SzType align, H::SzType size) {
  $raw_assert((align <= MI_ALIGNMENT_MAX) && H::is_power_of_2(align));
  return mi_aligned_alloc(align, size);
}

MIMALLOC_BASE::VoidAllocResult
 MIMALLOC_BASE::AllocateAtLeast(H::SzType size) {
  return { mi_malloc(size), size };
}

void MIMALLOC_BASE::Deallocate(void* p) 
{ mi_free(p); }

bool MIMALLOC_BASE::IsMallocRedirected() {
#if defined(PLATFORM_WINDOWS)
  return mi_is_redirected();
#else
  return false;
#endif
}

bool MIMALLOC_BASE::IsInHeapRegion(const void* p) {
  return mi_is_in_heap_region(p);
}

