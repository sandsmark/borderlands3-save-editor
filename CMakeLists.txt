cmake_minimum_required(VERSION 3.5)

project(borderlands3-save-editor LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt5 COMPONENTS Widgets REQUIRED)

find_package(Protobuf CONFIG REQUIRED)

find_package(ZLIB)

# wtf cmake... automoc?
qt5_wrap_cpp(moc_sources src/Lol.h)

protobuf_generate_cpp(protobufs_SRC protobufs_HDR
    src/protobufs/OakProfile.proto
    src/protobufs/OakSave.proto
    src/protobufs/OakShared.proto
    )

add_executable(borderlands3-save-editor
    src/main.cpp
    src/MainWindow.cpp
    src/Savegame.cpp
    src/Constants.cpp
    src/GeneralTab.cpp
    src/InventoryTab.cpp
    src/GameSettingsTab.cpp
    src/ConsumablesTab.cpp
    src/ItemData.cpp
    src/MissionsTab.cpp

    src/Lol.cpp

    ${protobufs_SRC}
    ${moc_sources}

    data.qrc
    )


target_link_libraries(borderlands3-save-editor PRIVATE Qt5::Widgets protobuf::libprotobuf)
