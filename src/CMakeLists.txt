SET(CORE_SOURCES
    surakarta_reason.cpp
    surakarta_game.cpp
    surakarta_rule_manager.cpp
    surakarta_rule_manager_impl.cpp
    global_random_generator.cpp
    surakarta_utils.cpp
    surakarta_agent_random.cpp
    surakarta_agent_mine.cpp
    surakarta_daemon.cpp
    surakarta_agent_interactive.cpp
    surakarta_animation.cpp
    surakarta_logger.cpp
)

if(NOT TARGET surakarta)
    if(WIN32)
        set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
    endif()
    add_library(surakarta SHARED ${CORE_SOURCES})
    set_target_properties(surakarta PROPERTIES POSITION_INDEPENDENT_CODE ON)
    target_include_directories(surakarta PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../include)
    install(TARGETS surakarta)
endif()

if(NOT TARGET surakarta-benchmark)
    add_executable(surakarta-benchmark main.cpp)
    target_link_libraries(surakarta-benchmark surakarta)
    add_test(NAME surakarta-benchmark COMMAND surakarta-benchmark statistic -n 10)
    install(TARGETS surakarta-benchmark)
endif()
