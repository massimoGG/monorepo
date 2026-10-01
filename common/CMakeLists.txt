cmake_minimum_required(VERSION 3.22)

target_sources(${CMAKE_PROJECT_NAME}
    PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/src/debug.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/ringbuffer.c"
)

target_include_directories(${CMAKE_PROJECT_NAME}
    PUBLIC
    inc/
)