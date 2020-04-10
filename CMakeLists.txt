cmake_minimum_required(VERSION 3.5)

project(borderlands3-save-editor LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt5 COMPONENTS Widgets REQUIRED)

find_package(Protobuf REQUIRED)

add_executable(borderlands3-save-editor
    main.cpp
    MainWindow.cpp
    Savegame.cpp
    Constants.cpp

    OakProfile.pb.cc
    OakSave.pb.cc
    OakShared.pb.cc
    )

target_link_libraries(borderlands3-save-editor PRIVATE Qt5::Widgets protobuf::libprotobuf)
