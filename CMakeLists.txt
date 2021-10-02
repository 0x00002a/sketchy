cmake_minimum_required(VERSION 3.1)

project(sketchy)

set(CMAKE_CXX_STANDARD 17)

set(SRC 
    "src/main.cpp"

    "src/ui/main_window.cpp"
    "src/ui/canvas.cpp"
)

set(EXE_NAME sketchy)

add_executable(${EXE_NAME} ${SRC})

find_package(Qt6 REQUIRED COMPONENTS Widgets Core)

target_link_libraries(${EXE_NAME} PUBLIC Qt6::Widgets Qt6::Core)

