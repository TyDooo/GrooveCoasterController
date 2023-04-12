cmake_minimum_required(VERSION 3.13)

# Pull in Raspberry Pi Pico SDK
include(lib/pico-sdk/pico_sdk_init.cmake)

project(PiCoaster2040)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

add_compile_options(-Wall -Wextra -Werror)

file(GLOB ${PROJECT_NAME}_SOURCES src/*.c)

add_executable(${PROJECT_NAME} ${${PROJECT_NAME}_SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_LIST_DIR})

target_link_libraries(${PROJECT_NAME} PUBLIC pico_stdlib)

pico_enable_stdio_usb(${PROJECT_NAME} 0)
pico_enable_stdio_uart(${PROJECT_NAME} 1)

# create map/bin/hex/uf2 file in addition to ELF.
pico_add_extra_outputs(${PROJECT_NAME})
