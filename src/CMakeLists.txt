SET(GAME_SOURCES surakarta_reason.cpp surakarta_game.cpp surakarta_rule_manager.cpp surakarta_rule_manager_impl.cpp global_random_generator.cpp surakarta_utils.cpp)
SET(AGENT_SOURCES surakarta_agent/surakarta_agent_random.cpp surakarta_agent/surakarta_agent_mine.cpp)

if(NOT TARGET surakarta)
    add_library(surakarta STATIC ${GAME_SOURCES} ${AGENT_SOURCES})
    set_target_properties(surakarta PROPERTIES POSITION_INDEPENDENT_CODE ON)
endif()

if(NOT TARGET libsurakarta-benchmark)
    add_executable(libsurakarta-benchmark main.cpp)
    target_link_libraries(libsurakarta-benchmark surakarta)
endif()
