cmake_minimum_required(VERSION 3.5)

project(borderlands3-save-editor LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt5 COMPONENTS Widgets REQUIRED)

if(MSVC)
    find_package(protobuf CONFIG REQUIRED VERSION "3.11.3.0")
else()
    find_package(Protobuf REQUIRED)
endif()

find_package(ZLIB)

add_executable(borderlands3-save-editor
    src/main.cpp
    src/MainWindow.cpp
    src/Savegame.cpp
    src/Constants.cpp
    src/generaltab.cpp
    src/InventoryTab.cpp
    src/GameSettingsTab.cpp

    src/OakProfile.pb.cc
    src/OakSave.pb.cc
    src/OakShared.pb.cc

    data.qrc
    )

target_link_libraries(borderlands3-save-editor PRIVATE Qt5::Widgets protobuf::libprotobuf)
