set(__EFL_CORE_SRCS
  "Panic/Handler.cpp"
  "MimAllocator.cpp"
  # ...
)

list(TRANSFORM __EFL_CORE_SRCS PREPEND "${CMAKE_CURRENT_SOURCE_DIR}/")
set(EFL_CORE_SRCS "${__EFL_CORE_SRCS}" PARENT_SCOPE)