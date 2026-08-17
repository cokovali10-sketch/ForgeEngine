cmake_minimum_required(VERSION 3.20)
project(ForgeEngine VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

option(FORGE_BUILD_TESTS "Build tests" ON)

find_package(SDL2 REQUIRED)

file(GLOB_RECURSE FORGE_SOURCES CONFIGURE_DEPENDS
  src/*.cpp
)

add_library(forge STATIC ${FORGE_SOURCES})
target_include_directories(forge PUBLIC include)
target_link_libraries(forge PUBLIC SDL2::SDL2)

target_compile_options(forge PRIVATE
  $<$<CXX_COMPILER_ID:GNU,Clang>:-Wall -Wextra -Wpedantic>
)

add_executable(demo examples/demo/main.cpp)
target_link_libraries(demo PRIVATE forge)
if(MINGW)
  target_link_options(demo PRIVATE -mwindows)
endif()

if(FORGE_BUILD_TESTS)
  enable_testing()
  file(GLOB FORGE_TESTS tests/*.cpp)
  foreach(test_src ${FORGE_TESTS})
    get_filename_component(test_name ${test_src} NAME_WE)
    add_executable(${test_name} ${test_src})
    target_link_libraries(${test_name} PRIVATE forge)
    add_test(NAME ${test_name} COMMAND ${test_name})
  endforeach()
endif()
