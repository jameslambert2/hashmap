
cmake_minimum_required(VERSION 3.16.3)
project(hashmap LANGUAGES "C" "CXX")

set(CMAKE_C_FLAGS "-g -Werror -Wall -pedantic")
set(CMAKE_INSTALL_RPATH ${hashmap_SOURCE_DIR}/bin)

# set(CMAKE_C_COMPILER gcc)
find_program(CLANG_TIDY_PROG clang-tidy)

if(CLANG_TIDY_PROG)
    set(CMAKE_C_CLANG_TIDY ${CLANG_TIDY_PROG} "--checks=*, -llvm-include-order,-cppcoreguidelines-*,-readability-magic-numbers,-clang-analyzer-security.insecureAPI.DeprecatedOrUnsafeBufferHandling")
endif()

include_directories(SYSTEM include)

add_subdirectory(src)
add_subdirectory(deps)