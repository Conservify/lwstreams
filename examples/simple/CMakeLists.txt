include(Arduino)

enable_arduino_toolchain()

file(GLOB sources *.cpp ../../src/*.cpp)

add_arduino_library(lwstreams-example-simple "${sources}")

include_directories(../../src)

find_package(SPI)

target_link_libraries(lwstreams-example-simple SPI)

target_compile_options(lwstreams-example-simple PRIVATE -Wall -Werror)

add_arduino_firmware(lwstreams-example-simple)
