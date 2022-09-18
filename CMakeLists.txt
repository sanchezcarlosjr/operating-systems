cmake_minimum_required(VERSION 3.10)

# set the project name
project(OperatingSystem)

set(CMAKE_CXX_STANDARD 20)

include(FetchContent)
FetchContent_Declare(
  fmt
  GIT_REPOSITORY https://github.com/fmtlib/fmt.git
  GIT_TAG 9.1.0
)

# For Windows: Prevent overriding the parent project's compiler/linker settings
set(fmt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(fmt)


# add the executable
add_executable(OperatingSystem fmt.c)

