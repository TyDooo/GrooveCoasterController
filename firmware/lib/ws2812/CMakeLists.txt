file(GLOB pio_ws2812_SOURCES src/*.c)

add_library(pio_ws2812 STATIC ${pio_ws2812_SOURCES})

file(MAKE_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/generated)

pico_generate_pio_header(pio_ws2812 ${CMAKE_CURRENT_LIST_DIR}/src/ws2812.pio
                         OUTPUT_DIR ${CMAKE_CURRENT_LIST_DIR}/generated)

target_include_directories(
  pio_ws2812
  PUBLIC ${CMAKE_CURRENT_LIST_DIR}/include
  PRIVATE ${CMAKE_CURRENT_LIST_DIR}/generated)

target_link_libraries(pio_ws2812 PUBLIC pico_stdlib hardware_pio)
