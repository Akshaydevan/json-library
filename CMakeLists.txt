cmake_minimum_required (VERSION 3.8)

project ("libjson")

set (CMAKE_CXX_STANDARD 20)
set (CMAKE_CXX_EXTENSIONS OFF)
set (CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB PROJECT_SOURCE_FILES *.cpp *.hpp)

add_library (libjson STATIC ${PROJECT_SOURCE_FILES})

target_include_directories (libjson PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_directories (libjson PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
