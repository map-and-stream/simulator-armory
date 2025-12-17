set(LIBRARY_NAME simulator_armory)

# Add radar sources/headers variables before using them
set(RADAR_SIMULATOR_SOURCES
    third-party/simulator/radar/radar_simulator.cpp
)
set(RADAR_SIMULATOR_HEADERS
    third-party/simulator/radar/radar_simulator.h
)

set(SIMULATOR_SOURCES general_simulator.cpp)
set(SIMULATOR_HEADERS simulator.h general_simulator.h)

# Optionally, append radar sources/headers to main lists
list(APPEND SIMULATOR_SOURCES ${RADAR_SIMULATOR_SOURCES})
list(APPEND SIMULATOR_HEADERS ${RADAR_SIMULATOR_HEADERS})

add_subdirectory(third-party/protocol-armory)
add_subdirectory(third-party/network-armory)

add_library(${LIBRARY_NAME} STATIC ${SIMULATOR_SOURCES})

target_link_libraries(${LIBRARY_NAME} PUBLIC protocol_armory network_armory)

# Keep headers associated with this target for IDEs, but do not propagate
target_sources(${LIBRARY_NAME} PRIVATE ${SIMULATOR_HEADERS})

target_include_directories(${LIBRARY_NAME} PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/third-party
    ${CMAKE_CURRENT_SOURCE_DIR}/third-party/simulator/radar
)

# Convenient namespaced alias for downstream targets
add_library(${LIB_ALIAS} ALIAS ${LIBRARY_NAME})
