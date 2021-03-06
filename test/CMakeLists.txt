cmake_minimum_required(VERSION 2.8)

# GTest requires this.
find_package(Threads REQUIRED)

# Enable ExternalProject CMake module
include(ExternalProject)

# Download and install GoogleTest
ExternalProject_Add(
    gtest
    URL https://github.com/google/googletest/archive/release-1.8.0.zip
    PREFIX ${CMAKE_CURRENT_BINARY_DIR}/gtest
    # Disable install step
    INSTALL_COMMAND ""
)

# Get GTest source and binary directories from CMake project
ExternalProject_Get_Property(gtest source_dir binary_dir)

# Create a libgtest target to be used as a dependency by test programs
add_library(libgtest IMPORTED STATIC GLOBAL)
add_dependencies(libgtest gtest)

# Set libgtest properties
set_target_properties(libgtest PROPERTIES
  "IMPORTED_LOCATION" "${binary_dir}/googlemock/gtest/libgtest.a"
  "IMPORTED_LINK_INTERFACE_LIBRARIES" "${CMAKE_THREAD_LIBS_INIT}"
)

# Create a libgmock target to be used as a dependency by test programs
add_library(libgmock IMPORTED STATIC GLOBAL)
add_dependencies(libgmock gtest)

# Set libgmock properties
set_target_properties(libgmock PROPERTIES
  "IMPORTED_LOCATION" "${binary_dir}/googlemock/libgmock.a"
  "IMPORTED_LINK_INTERFACE_LIBRARIES" "${CMAKE_THREAD_LIBS_INIT}"
)

# I couldn't make it work with INTERFACE_INCLUDE_DIRECTORIES
include_directories("${source_dir}/googletest/include"
                    "${source_dir}/googlemock/include")


# Download and install GoogleTest
ExternalProject_Add(
    nanopb
    GIT_REPOSITORY https://github.com/nanopb/nanopb.git
    PREFIX ${CMAKE_CURRENT_BINARY_DIR}/nanopb
    # Disable install step
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ""
)
ExternalProject_Get_Property(nanopb source_dir binary_dir)

add_custom_command(OUTPUT ${source_dir}/pb_common.c DEPENDS nanopb COMMAND "")
add_custom_command(OUTPUT ${source_dir}/pb_decode.c DEPENDS nanopb COMMAND "")
add_custom_command(OUTPUT ${source_dir}/pb_encode.c DEPENDS nanopb COMMAND "")

file(GLOB SRCS *.cpp protocol/*.c ../src/lwstreams/*)

add_executable(testcommon ${SRCS} ${source_dir}/pb_common.c ${source_dir}/pb_decode.c ${source_dir}/pb_encode.c)

add_definitions(-DLWS_ENABLE_PROTOBUF)

target_include_directories(testcommon PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(testcommon PUBLIC ${source_dir})
target_include_directories(testcommon PUBLIC "../src")

target_link_libraries(testcommon libgtest libgmock)

set_target_properties(testcommon PROPERTIES C_STANDARD 11)
set_target_properties(testcommon PROPERTIES CXX_STANDARD 11)

add_test(NAME testcommon COMMAND testcommon)
