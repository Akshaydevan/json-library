cmake_minimum_required (VERSION 3.8)

project ("libjson")

set (CMAKE_CXX_STANDARD 17)
set (CMAKE_CXX_EXTENSIONS OFF)
set (CMAKE_CXX_STANDARD_REQUIRED ON)

add_library (libjson STATIC
             lex.cpp
             lex.hpp
             parse.cpp
             parse.hpp
             map.hpp
             json.cpp
             include/libjson/json.hpp
            )
