cmake_minimum_required(VERSION 3.23)

project(
    sequence
    VERSION 1.1.0
    DESCRIPTION "Microtonal step sequencer library"
    LANGUAGES CXX
)

include(GNUInstallDirs)

if(PROJECT_IS_TOP_LEVEL)
    include(CTest)
endif()

set(sequence_build_tests_default OFF)
if(PROJECT_IS_TOP_LEVEL AND BUILD_TESTING)
    set(sequence_build_tests_default ON)
endif()
option(SEQUENCE_BUILD_TESTS "Build MicrotonalStepSequencer tests"
       ${sequence_build_tests_default})
unset(sequence_build_tests_default)

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

if(SEQUENCE_BUILD_TESTS)
    enable_testing()
    add_executable(sequence_tests
        test/catch.main.cpp
        test/measure.test.cpp
        test/midi.test.cpp
        test/modify.test.cpp
        test/pattern.test.cpp
        test/test.cpp
    )
    target_link_libraries(sequence_tests PRIVATE sequence::sequencer)
    add_test(NAME sequencer_tests COMMAND sequence_tests)
endif()

install(
    TARGETS sequencer
    EXPORT sequenceTargets
    FILE_SET HEADERS DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

install(
    EXPORT sequenceTargets
    FILE sequenceTargets.cmake
    NAMESPACE sequence::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/sequence
)

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/sequenceConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)
configure_package_config_file(
    cmake/sequenceConfig.cmake.in
    "${CMAKE_CURRENT_BINARY_DIR}/sequenceConfig.cmake"
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/sequence
)
install(
    FILES
        "${CMAKE_CURRENT_BINARY_DIR}/sequenceConfig.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/sequenceConfigVersion.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/sequence
)
