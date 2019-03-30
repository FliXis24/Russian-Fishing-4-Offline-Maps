project(Russian-Fishing-4-Offline-Maps)
cmake_minimum_required(VERSION 3.10.2)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic -pedantic-errors")
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5 REQUIRED Core Widgets Multimedia)

set(SOURCE_FILES
    "main.cpp"
    "MainWindow.cpp"
    "MapsManager.cpp"
    "MapSetup.cpp"
    "Map.cpp"
    "CreatePoint.cpp"
    "QLabelEventGetPoint.cpp"
    "WaterBody.cpp"
    "Point.cpp"
    "FoundPoint.cpp"
    "License.cpp"
    "QLabelClearTextTimer.cpp"
    "About.cpp"
)
set(HEADER_FILES
    "MainWindow.h"
    "QLabelEventGetPoint.h"
    "WaterBody.h"
    "MapsManager.h"
    "MapSetup.h"
    "Map.h"
    "CreatePoint.h"
    "Point.h"
    "FoundPoint.h"
    "License.h"
    "QLabelClearTextTimer.h"
    "About.h"
)
set(UI_FILES
    "MainWindow.ui"
    "MapsManager.ui"
    "MapSetup.ui"
    "CreatePoint.ui"
    "License.ui"
    "Point.ui"
    "About.ui"
)
set(QRC_FILES
    "resource.qrc"
)

add_executable(${PROJECT_NAME} ${SOURCE_FILES} ${HEADER_FILES} ${UI_FILES} ${QRC_FILES})

target_link_libraries(${PROJECT_NAME} Qt5::Core Qt5::Widgets Qt5::Multimedia)

configure_file(${PROJECT_SOURCE_DIR}/resource/sound/leaf.wav ${PROJECT_BINARY_DIR}/sound/leaf.wav COPYONLY)
configure_file(${PROJECT_SOURCE_DIR}/resource/sound/negative.wav ${PROJECT_BINARY_DIR}/sound/negative.wav COPYONLY)
configure_file(${PROJECT_SOURCE_DIR}/resource/sound/positive.wav ${PROJECT_BINARY_DIR}/sound/positive.wav COPYONLY)
configure_file(${PROJECT_SOURCE_DIR}/resource/sound/switch.wav ${PROJECT_BINARY_DIR}/sound/switch.wav COPYONLY)
configure_file(${PROJECT_SOURCE_DIR}/resource/sound/warning.wav ${PROJECT_BINARY_DIR}/sound/warning.wav COPYONLY)
