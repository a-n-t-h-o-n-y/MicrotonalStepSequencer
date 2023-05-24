cmake_minimum_required(VERSION 3.16)

project(sequence)

# Header Only Library
add_library(sequencer INTERFACE)
target_include_directories(sequencer
    INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)
target_compile_features(sequencer
    INTERFACE
        cxx_std_20
)
target_compile_options(sequencer INTERFACE
  $<$<CXX_COMPILER_ID:MSVC>:/W1>
  $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic>
)

# Add your test cpp file
add_executable(tests
    test/catch.main.cpp
    test/modify.test.cpp
    test/measure.test.cpp
    test/midi.test.cpp
    test/test.cpp
)
target_link_libraries(tests sequencer)