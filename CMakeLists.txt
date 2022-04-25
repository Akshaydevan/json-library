cmake_minimum_required (VERSION 3.8)

project ("libjson")

set (CMAKE_CXX_STANDARD 17)
set (CMAKE_CXX_EXTENSIONS OFF)
set (CMAKE_CXX_STANDARD_REQUIRED ON)

add_library (libjson STATIC
             src/lex.cpp
             include/libjson/lex.hpp
             src/parse.cpp
             include/libjson/parse.hpp
             src/map.cpp
             include/libjson/map.hpp
             src/json.cpp
             include/libjson/json.hpp
            )

if(BUILD_TESTING)
    enable_testing()
    file(COPY "tests/json files/" DESTINATION "${CMAKE_BINARY_DIR}/libjson/tests/json files/")
    add_subdirectory(tests)
endif()
