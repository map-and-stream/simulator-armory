cmake_minimum_required(VERSION 3.10)
project(simulator_armory)

set(CMAKE_CXX_STANDARD 20)

# -----------------------------------
# Example app: radar
# -----------------------------------
add_executable(radar
    radar/main.cpp
    
)

target_include_directories(radar PRIVATE
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/src/third-party
    ${CMAKE_SOURCE_DIR}/third-party/simulator/radar
)

target_link_libraries(radar PRIVATE ${LIB_ALIAS})

# -----------------------------------
# Example app: lrad_server
# -----------------------------------
add_executable(lrad
    lrad/main.cpp

)

target_include_directories(lrad PRIVATE
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/src/third-party
    ${CMAKE_SOURCE_DIR}/third-party/simulator/lrad
)

target_link_libraries(lrad PRIVATE ${LIB_ALIAS})

