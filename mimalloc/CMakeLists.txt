include_guard(DIRECTORY)

set(MI_OVERRIDE ${__EFL_MIMALLOC_NEW})
set(MI_BUILD_TESTS OFF)
set(MI_USE_CXX ON)
set(MI_BUILD_SHARED ON)
set(MI_BUILD_STATIC OFF)
# Currently doesn't work on windows.
set(MI_WIN_REDIRECT OFF)

if(CMAKE_BUILD_TYPE EQUAL "DEBUG")
  set(MI_PADDING ON)
  set(MI_TRACK_ETW ON)
endif()

add_subdirectory(mimalloc)
