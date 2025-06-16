cmake_minimum_required(VERSION 3.25)

# Need to include pico-sdk cmake support. Must happen before project.
include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)

project(PiCoaster2040
  LANGUAGES C CXX ASM
)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

file(GLOB_RECURSE ${PROJECT_NAME}_C_SOURCES
    CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c"
)

file(GLOB_RECURSE ${PROJECT_NAME}_H_HEADERS
    CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h"
)

add_executable(${PROJECT_NAME}
    ${${PROJECT_NAME}_C_SOURCES}
    ${${PROJECT_NAME}_H_HEADERS}
)

target_include_directories(${PROJECT_NAME}
                           PRIVATE
                           ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(${PROJECT_NAME} PRIVATE pico_stdlib tinyusb_device
                                              tinyusb_board)

pico_enable_stdio_usb(${PROJECT_NAME} 1)
pico_enable_stdio_uart(${PROJECT_NAME} 1)

# create map/bin/hex/uf2 file in addition to ELF.
pico_add_extra_outputs(${PROJECT_NAME})
