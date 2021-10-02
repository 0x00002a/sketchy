cmake_minimum_required(VERSION 3.1)

project(sketchy)

set(CMAKE_CXX_STANDARD 20)

option(SKETCHY_BUILD_TESTS "Whether to build the tests" OFF)

include(${CMAKE_CURRENT_LIST_DIR}/conan.cmake)

find_package(Qt6 REQUIRED COMPONENTS Widgets Core Svg)

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
    "src/storage.cpp"

    "src/ui/main_window.cpp"
    "src/ui/canvas.cpp"
    "src/ui/infinite_scroll.cpp"
)

set(EXE_NAME sketchy)
set(LIB_NAME libsketchy)

add_library(${LIB_NAME} ${SRC})

if (${CMAKE_CXX_COMPILER_ID} MATCHES "(GNU|AppleClang|Clang)")
    target_compile_options(
        ${LIB_NAME}
        PUBLIC
            -fdiagnostics-color=always
    )
endif()



find_package(spdlog REQUIRED)

target_link_libraries(${LIB_NAME} PUBLIC Qt6::Widgets Qt6::Core Qt6::Svg spdlog::spdlog cronch)
target_include_directories(${LIB_NAME} PUBLIC "./src")

if (SKETCHY_BUILD_TESTS) 
    add_subdirectory(test)
endif()


add_executable(${EXE_NAME} "src/main.cpp")
target_link_libraries(${EXE_NAME} ${LIB_NAME})