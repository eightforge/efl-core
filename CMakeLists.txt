cmake_minimum_required(VERSION 3.11)
include_guard(DIRECTORY)

project(
  efl-core
  VERSION 0.1.1
  LANGUAGES CXX
)

option(EFL_CORE_PANICGUARD "Use a mutex for the panic handler." ON)
option(EFL_CORE_PANICSINGLE "Only allow the panic handler to be set once." OFF)
option(EFL_CORE_TESTING "Enable testing for efl::core." OFF)

message("[efl::core] panic-guard: ${EFL_CORE_PANICGUARD}")
message("[efl::core] panic-single: ${EFL_CORE_PANICSINGLE}")

if(EFL_CORE_TESTING)
  message("[efl::core] core-tests: ${EFL_CORE_TESTING}")
else()
  message(DEBUG "[efl::core] core-tests: ${EFL_CORE_TESTING}")
endif()

set(CMAKE_CXX_STANDARD 20)
add_subdirectory(src) # EFL_CORE_SRCS
include(EflGetModules)
Efl_Getmodules(efl::config)

add_library(__efl_core STATIC ${EFL_CORE_SRCS})
target_link_libraries(__efl_core PUBLIC efl::config)
target_include_directories(__efl_core PUBLIC include)

if(EFL_CORE_PANICGUARD)
  target_compile_definitions(__efl_core PRIVATE EFLI_PANICGUARD_=1)
endif()

if(EFL_CORE_PANICSINGLE)
  target_compile_definitions(__efl_core PRIVATE EFLI_PANICSINGLE_=1)
endif()

target_include_directories(__efl_core PRIVATE include/efl)
target_compile_definitions(__efl_core PUBLIC EFL_CORE_VERSION="${PROJECT_VERSION}")

add_library(efl::core ALIAS __efl_core)

add_executable(efl-core-tests Tests.cpp)
target_link_libraries(efl-core-tests efl::core)