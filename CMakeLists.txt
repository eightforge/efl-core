cmake_minimum_required(VERSION 3.16)
include_guard(DIRECTORY)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

project(
  efl-core
  VERSION 0.3.0
  LANGUAGES CXX
)

option(EFL_MULTITHREADED "Enable multithreading for the efl library." ON)
option(EFL_MIMALLOC_NEW "Use mimalloc as the default allocator." ON)

option(EFL_CORE_TESTING "Enable testing for efl::core." OFF)
option(EFL_CORE_PANICGUARD "Use a mutex for the panic handler." ON)
option(EFL_CORE_PANICSINGLE "Only allow the panic handler to be set once." OFF)

set(__EFL_MULTITHREADED ${EFL_MULTITHREADED} CACHE
  BOOL "Caches multithreaded selection to avoid odr violations.")
set(__EFL_MIMALLOC_NEW ${EFL_MIMALLOC_NEW} CACHE
  BOOL "Caches allocator selection to avoid odr violations.")

message("[efl::core] is-multithreaded: ${__EFL_MULTITHREADED}")
message("[efl::core] mimalloc-new: ${__EFL_MIMALLOC_NEW}")
message("[efl::core] panic-guard: ${EFL_CORE_PANICGUARD}")
message("[efl::core] panic-single: ${EFL_CORE_PANICSINGLE}")

if(EFL_CORE_TESTING)
  message("[efl::core] core-tests: ${EFL_CORE_TESTING}")
else()
  message(DEBUG "[efl::core] core-tests: ${EFL_CORE_TESTING}")
endif()

add_subdirectory(mimalloc) # mimalloc
add_subdirectory(src) # EFL_CORE_SRCS
include(EflGetModules)
Efl_Getmodules(efl::config)

add_library(__efl_core STATIC ${EFL_CORE_SRCS})
target_link_libraries(__efl_core PUBLIC efl::config)
target_link_libraries(__efl_core PRIVATE mimalloc)
target_include_directories(__efl_core PUBLIC include)
target_compile_features(__efl_core PRIVATE cxx_std_11)

target_compile_definitions(__efl_core PUBLIC "EFL_MULTITHREADED=$<BOOL:${__EFL_MULTITHREADED}>")
target_compile_definitions(__efl_core PUBLIC "EFL_MIMALLOC_NEW=$<BOOL:${__EFL_MIMALLOC_NEW}>")
target_compile_definitions(__efl_core PRIVATE "EFLI_PANICGUARD_=$<BOOL:${EFL_CORE_PANICGUARD}>")
target_compile_definitions(__efl_core PRIVATE "EFLI_PANICSINGLE_=$<BOOL:${EFL_CORE_PANICSINGLE}>")

target_include_directories(__efl_core PRIVATE include/efl)
target_compile_definitions(__efl_core PUBLIC EFL_CORE_VERSION="${PROJECT_VERSION}")

add_library(efl::core ALIAS __efl_core)

if(EFL_CORE_TESTING)
  add_executable(efl-core-tests Tests.cpp)
  target_link_libraries(efl-core-tests efl::core)
endif()