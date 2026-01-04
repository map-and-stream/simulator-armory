cmake_minimum_required(VERSION 3.16)

project(ui_simulator VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets Network)

qt_standard_project_setup()

add_executable(ui_simulator
    main.cpp
    src/RadarClient.cpp
    src/RadarWindow.cpp
    src/RadarWidget.cpp
    src/RadarClient.h
    src/RadarWidget.h
    src/RadarWindow.h
)

# Include protocol-armory headers
target_include_directories(ui_simulator PRIVATE
    src
    /home/isiran/Desktop/protocol-armory/src
)

# Link protocol-armory static library
target_link_libraries(ui_simulator PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Network
    /home/isiran/Desktop/protocol-armory/build/src/libprotocol_armory.a
)


