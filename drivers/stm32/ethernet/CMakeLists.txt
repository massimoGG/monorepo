cmake_minimum_required(VERSION 3.11)

target_sources(${CMAKE_PROJECT_NAME} PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/src/ethernet_descriptors.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/ethernet_ll.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/mmc.c"
)

target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC
    inc/
)
