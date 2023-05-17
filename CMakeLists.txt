cmake_minimum_required(VERSION 3.16)

project(sequence)

# Header Only Library
add_library(sequencer INTERFACE)
target_include_directories(sequencer
    INTERFACE
        include
)
target_compile_features(sequencer
    INTERFACE
        cxx_std_20
)
target_compile_options(sequencer
    INTERFACE
        -Wall
        -Wextra
        -Wpedantic
        -Werror
)

# Add your test cpp file
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/test)
add_executable(tests
    test/catch.main.cpp
    test/test.cpp
)
target_link_libraries(tests sequencer)