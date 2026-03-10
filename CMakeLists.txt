cmake_minimum_required(VERSION 3.23)

project(
    sequence
    VERSION 0.1.0
    DESCRIPTION "Microtonal step sequencer library"
    LANGUAGES CXX
)

include(CTest)

add_library(sequencer STATIC)
add_library(sequence::sequencer ALIAS sequencer)

target_compile_features(sequencer PUBLIC cxx_std_20)

target_include_directories(sequencer
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

target_compile_options(sequencer
    PRIVATE
        $<$<CXX_COMPILER_ID:MSVC>:/W4>
        $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic>
)

set_target_properties(sequencer PROPERTIES POSITION_INDEPENDENT_CODE ON)

target_sources(sequencer
    PRIVATE
        src/generate.cpp
        src/midi.cpp
        src/modify.cpp
        src/pattern.cpp
        src/time_signature.cpp
        src/timing.cpp
        src/tuning.cpp
    PUBLIC
        FILE_SET HEADERS
        BASE_DIRS include
        FILES
            include/sequence/generate.hpp
            include/sequence/midi.hpp
            include/sequence/modify.hpp
            include/sequence/pattern.hpp
            include/sequence/random.hpp
            include/sequence/sequence.hpp
            include/sequence/time_signature.hpp
            include/sequence/timing.hpp
            include/sequence/tuning.hpp
            include/sequence/utility.hpp
)

if(BUILD_TESTING)
    add_executable(tests
        test/catch.main.cpp
        test/measure.test.cpp
        test/midi.test.cpp
        test/modify.test.cpp
        test/test.cpp
    )
    target_link_libraries(tests PRIVATE sequence::sequencer)
    add_test(NAME sequencer_tests COMMAND tests)
endif()
