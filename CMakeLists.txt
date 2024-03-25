cmake_minimum_required(VERSION 3.16)

project(libsurakarta)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# add_compile_options(-fsanitize=address)
# add_compile_options(-g)
# add_link_options(-fsanitize=address)

set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS "-Ofast")

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include) 
add_subdirectory(src)
if(CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64" AND CMAKE_SYSTEM_NAME STREQUAL "Linux")
    add_subdirectory(test)
endif()
