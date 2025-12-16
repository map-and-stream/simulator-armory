cmake_minimum_required(VERSION 3.10)
project(simulator_armory_example)

set(CMAKE_CXX_STANDARD 20)

add_executable(radar_server radar/radar_server.cpp)
add_executable(lrad_server lrad/lrad_server.cpp)
add_executable(server server/server.cpp)

target_link_libraries(radar_server PRIVATE ${LIB_ALIAS})
target_link_libraries(lrad_server PRIVATE ${LIB_ALIAS})
target_link_libraries(server PRIVATE ${LIB_ALIAS})

target_include_directories(radar_server PRIVATE
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/src/third-party
)
target_include_directories(lrad_server PRIVATE
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/src/third-party
)
target_include_directories(server PRIVATE
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/src/third-party
)
