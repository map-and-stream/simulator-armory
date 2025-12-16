set(LIBRARY_NAME simulator_armory)

set(SIMULATOR_SOURCES general_simulator.cpp)
set(SIMULATOR_HEADERS simulator.h general_simulator.h)

add_subdirectory(third-party/protocol-armory)
add_subdirectory(third-party/network-armory)


add_library(${LIBRARY_NAME} STATIC ${SIMULATOR_SOURCES})

# define public in order we cane use library in test foremove commentlder
target_link_libraries(${LIBRARY_NAME} PUBLIC protocol_armory network_armory)

# Keep headers associated with this target for IDEs, but do not propagate
target_sources(${LIBRARY_NAME} PRIVATE ${SIMULATOR_HEADERS})

target_include_directories(${LIBRARY_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/third-party)

# Convenient namespaced alias for downstream targets
add_library(${LIB_ALIAS} ALIAS ${LIBRARY_NAME})
