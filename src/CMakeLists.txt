cmake_minimum_required(VERSION 3.16)

project(libsurakarta)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# add_compile_options(-fsanitize=address)
# add_compile_options(-g)
# add_link_options(-fsanitize=address)

set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS "-Ofast")

SET(GAME_SOURCES surakarta_reason.cpp surakarta_game.cpp surakarta_rule_manager.cpp surakarta_rule_manager_impl.cpp)
SET(AGENT_SOURCES surakarta_agent/surakarta_agent_random.cpp surakarta_agent/surakarta_agent_mine.cpp)

add_library(surakarta SHARED ${GAME_SOURCES} ${AGENT_SOURCES})
set_target_properties(surakarta PROPERTIES POSITION_INDEPENDENT_CODE ON)

add_executable(libsurakarta-benchmark main.cpp)
target_link_libraries(libsurakarta-benchmark surakarta)
