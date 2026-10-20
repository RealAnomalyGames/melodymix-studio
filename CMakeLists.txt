cmake_minimum_required(VERSION 3.21)

project(MelodyMixStudio
    VERSION 0.1.0
    DESCRIPTION "A Windows 95-inspired industrial step sequencer"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets Multimedia)

qt_standard_project_setup()

qt_add_executable(MelodyMixStudio
    src/main.cpp
)

target_link_libraries(MelodyMixStudio
    PRIVATE
        Qt6::Widgets
        Qt6::Multimedia
)

target_compile_definitions(MelodyMixStudio
    PRIVATE
        MELODY_MIX_VERSION="${PROJECT_VERSION}"
)

if(WIN32)
    set_target_properties(MelodyMixStudio PROPERTIES
        WIN32_EXECUTABLE TRUE
    )
endif()
