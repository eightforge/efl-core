cmake_minimum_required(VERSION 3.13)
include_guard(DIRECTORY)

project(
  efl-core
  VERSION 0.1.0
  LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
include(EflGetModules)
Efl_Getmodules(efl::config)

add_library(__efl_core INTERFACE)
target_link_libraries(__efl_core INTERFACE efl::config)
target_include_directories(__efl_core INTERFACE include)

add_library(efl::core ALIAS __efl_core)

add_executable(efl-core-tests Tests.cpp)
target_link_libraries(efl-core-tests efl::core)