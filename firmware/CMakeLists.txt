cmake_minimum_required(VERSION 3.13)

# initialize the SDK based on PICO_SDK_PATH
include(pico_sdk_import.cmake)

project(PiCoaster2040)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

add_compile_options(-Wall -Wextra -Werror)

file(GLOB ${PROJECT_NAME}_SOURCES src/*.c src/usb/*.c)

add_executable(${PROJECT_NAME} ${${PROJECT_NAME}_SOURCES})

target_include_directories(${PROJECT_NAME}
                           PRIVATE ${CMAKE_CURRENT_LIST_DIR}/include)

target_link_libraries(${PROJECT_NAME} PRIVATE pico_stdlib tinyusb_device
                                              tinyusb_board)

pico_enable_stdio_usb(${PROJECT_NAME} 0)
pico_enable_stdio_uart(${PROJECT_NAME} 1)

# create map/bin/hex/uf2 file in addition to ELF.
pico_add_extra_outputs(${PROJECT_NAME})
