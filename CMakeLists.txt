cmake_minimum_required(VERSION 3.1)

project(sketchy)

set(CMAKE_CXX_STANDARD 20)

include(${CMAKE_CURRENT_LIST_DIR}/conan.cmake)

find_package(Qt6 REQUIRED COMPONENTS Widgets Core)

include(FetchContent)
FetchContent_Declare(
    cronch
    GIT_REPOSITORY https://github.com/0x00002a/cronch
    GIT_TAG        main
)
FetchContent_GetProperties(cronch)
if (NOT cronch_POPULATED) 
    FetchContent_Populate(cronch)

    add_subdirectory(${cronch_SOURCE_DIR} ${cronch_BINARY_DIR})
endif()


set(CMAKE_AUTOMOC ON)

set(SRC 
    "src/main.cpp"

    "src/ui/main_window.cpp"
    "src/ui/canvas.cpp"
    "src/ui/infinite_scroll.cpp"
)

set(EXE_NAME sketchy)

add_executable(${EXE_NAME} ${SRC})

find_package(spdlog REQUIRED)

target_link_libraries(${EXE_NAME} PUBLIC Qt6::Widgets Qt6::Core spdlog::spdlog cronch)
target_include_directories(${EXE_NAME} PUBLIC "./src")
