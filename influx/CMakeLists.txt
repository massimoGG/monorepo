cmake_minimum_required(VERSION 3.22)

target_sources(${CMAKE_PROJECT_NAME}
    PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/src/influx.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/influxline.cpp"
)

target_include_directories(${CMAKE_PROJECT_NAME}
    PUBLIC
    inc/
)

# Require LIBCURL
target_link_libraries(
    ${CMAKE_PROJECT_NAME}
    PRIVATE
    CURL::libcurl
)
