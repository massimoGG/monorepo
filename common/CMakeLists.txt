cmake_minimum_required(VERSION 3.22)

target_sources(${CMAKE_PROJECT_NAME}
    PUBLIC
    "src/debug.c"
    "src/ringbuffer.c"
)

target_include_directories(${CMAKE_PROJECT_NAME}
    PUBLIC
    inc/
)