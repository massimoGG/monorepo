cmake_minimum_required(VERSION 3.16)

target_sources(${CMAKE_PROJECT_NAME} PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/lan8742a.c"
)

target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC
    "."
)