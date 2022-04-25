cmake_minimum_required (VERSION 3.8)

project ("tests")

set (CMAKE_CXX_STANDARD 17)
set (CMAKE_CXX_EXTENSIONS OFF)
set (CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

add_executable(tests
               tests.cpp
)

target_include_directories(tests PRIVATE "../include/")
target_link_libraries(tests PRIVATE libjson gtest_main)

include(GoogleTest)
gtest_discover_tests(tests)
