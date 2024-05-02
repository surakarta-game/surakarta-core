# if gtest not added:
if(NOT TARGET gtest)
    add_subdirectory(../third-party/googletest ${CMAKE_BINARY_DIR}/third-party/googletest/googletest EXCLUDE_FROM_ALL)
endif()

get_filename_component(TEST_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}" ABSOLUTE)
add_definitions(-DTEST_DATA_DIR="${TEST_SOURCE_DIR}/test_data/")

find_library(SURAKARTA_TA_LIB surakarta_ta_lib ../third-party/panjd123/surakarta_ta/lib)
add_library(surakarta_ta_lib SHARED IMPORTED)
set_target_properties(surakarta_ta_lib PROPERTIES IMPORTED_LOCATION ${SURAKARTA_TA_LIB})

if(NOT TARGET surakarta-test)
    add_executable(surakarta-test surakarta_rule_manager_test.cpp)
    target_link_libraries(surakarta-test PRIVATE surakarta surakarta_ta_lib gtest gtest_main)
    add_test(NAME surakarta-test COMMAND surakarta-test)
endif()

# When using clang, -Wdelete-non-abstract-non-virtual-dtor is given.
# This is completely necessary, but SurakartaRuleManagerImp is close source, so it is not possible to fix it.
# Therefore, this warning is disabled.
if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    target_compile_options(surakarta-test PRIVATE -Wno-delete-non-abstract-non-virtual-dtor)
endif()
