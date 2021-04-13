# TODO Add static/shared build option and find_library support

cmake_minimum_required(VERSION 3.15)

project(io_proj LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)

find_package(GTest REQUIRED)
find_package(spdlog REQUIRED)
find_package(fmt REQUIRED)

# Warnings

# TODO: Add for other compilers

if (CMAKE_CXX_COMPILER_ID MATCHES "GNU")
  set(PEDANTIC_COMPILE_FLAGS -pedantic-errors -Wall -Wextra -pedantic
      -Wold-style-cast -Wundef
      -Wredundant-decls -Wwrite-strings -Wpointer-arith
      -Wcast-qual -Wformat=2 -Wmissing-include-dirs
      -Wcast-align
      -Wctor-dtor-privacy -Wdisabled-optimization
      -Winvalid-pch -Woverloaded-virtual
      -Wconversion -Wswitch-enum -Wundef
      -Wno-ctor-dtor-privacy -Wno-format-nonliteral)
  if (NOT CMAKE_CXX_COMPILER_VERSION VERSION_LESS 4.6)
      set(PEDANTIC_COMPILE_FLAGS ${PEDANTIC_COMPILE_FLAGS} -Wnoexcept
         -Wno-dangling-else -Wno-unused-local-typedefs)
  endif ()
  if (NOT CMAKE_CXX_COMPILER_VERSION VERSION_LESS 5.0)
      set(PEDANTIC_COMPILE_FLAGS ${PEDANTIC_COMPILE_FLAGS} -Wdouble-promotion
          -Wtrampolines -Wzero-as-null-pointer-constant -Wuseless-cast
          -Wvector-operation-performance -Wsized-deallocation -Wshadow)
  endif ()
  if (NOT CMAKE_CXX_COMPILER_VERSION VERSION_LESS 6.0)
      set(PEDANTIC_COMPILE_FLAGS ${PEDANTIC_COMPILE_FLAGS} -Wshift-overflow=2
          -Wnull-dereference -Wduplicated-cond)
  endif ()
  set(WERROR_FLAG -Werror)
endif ()

# Subprojects

# Library
add_library(io)

target_sources(io PRIVATE
    src/io/serial.cc
    src/io/device.cc
    src/io/pseudo_serial.cc
)

target_include_directories(io PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

# Library
add_library(xmodem)

target_sources(xmodem PRIVATE
    src/xmodem/packet.cc
    src/xmodem/endpoint.cc
    src/xmodem/checksum.cc
    src/xmodem/crc.cc
    src/xmodem/receiver.cc
    src/xmodem/transmitter.cc
)

target_include_directories(xmodem PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_link_libraries(xmodem
    spdlog::spdlog
    io
)

# Executable
add_executable(interface)

target_sources(interface PRIVATE
    src/interface/main.cc
)

target_include_directories(interface PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_link_libraries(interface
    io
    xmodem
    fmt::fmt
)

# Testing
enable_testing()

add_executable(simple_test)

target_sources(simple_test PRIVATE
    test/pseudo_serial_test.cc
)

target_link_libraries(simple_test
    GTest::gtest_main
    io
    xmodem
)

target_include_directories(simple_test PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

include(GoogleTest)

gtest_discover_tests(simple_test)

