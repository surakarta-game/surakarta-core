cmake_minimum_required(VERSION 3.16)

project(libsurakarta-test)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_subdirectory(../third-party/googletest ${CMAKE_BINARY_DIR}/third-party/googletest)

# add_compile_options(-fsanitize=address)
# add_compile_options(-g)
# add_link_options(-fsanitize=address)

set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS "-Ofast")

get_filename_component(TEST_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}" ABSOLUTE)
add_definitions(-DTEST_DATA_DIR="${TEST_SOURCE_DIR}/test_data/")

find_library(SURAKARTA_TA_LIB surakarta_ta_lib ${CMAKE_SOURCE_DIR}/third-party/panjd123/surakarta_ta/lib)
add_library(surakarta_ta_lib SHARED IMPORTED)
set_target_properties(surakarta_ta_lib PROPERTIES IMPORTED_LOCATION ${SURAKARTA_TA_LIB})

add_executable(libsurakarta-test surakarta_rule_manager_test.cpp)
target_link_libraries(libsurakarta-test PRIVATE surakarta surakarta_ta_lib gtest gtest_main)

# When using clang, -Wdelete-non-abstract-non-virtual-dtor is given.
# This is completely necessary, but SurakartaRuleManagerImp is close source, so it is not possible to fix it.
# Therefore, this warning is disabled.
if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    target_compile_options(libsurakarta-test PRIVATE -Wno-delete-non-abstract-non-virtual-dtor)
endif()
