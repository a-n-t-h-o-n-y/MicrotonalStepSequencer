cmake_minimum_required(VERSION 3.16)

project(sequence)

# Regular Library
add_library(sequencer STATIC)

target_include_directories(sequencer
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)
target_compile_features(sequencer
    PUBLIC
        cxx_std_20
)
target_compile_options(sequencer PUBLIC
  $<$<CXX_COMPILER_ID:MSVC>:/W1>
  $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic>
)

set_target_properties(sequencer PROPERTIES POSITION_INDEPENDENT_CODE ON)

target_sources(sequencer
    PRIVATE
        src/generate.cpp
        src/measure.cpp
        src/midi.cpp
        src/modify.cpp
        src/tuning.cpp
)

add_executable(tests
    test/catch.main.cpp
    test/modify.test.cpp
    test/measure.test.cpp
    test/midi.test.cpp
    test/test.cpp
)
target_link_libraries(tests sequencer)
