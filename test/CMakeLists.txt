
set(SRC
    "main.cpp"
) 

add_executable(tests ${SRC})

find_package(doctest REQUIRED)

target_link_libraries(tests doctest::doctest ${LIB_NAME})

target_compile_definitions(tests PRIVATE DOCTEST_CONFIG_IMPLEMENT)
