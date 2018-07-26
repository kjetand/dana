include_directories(${DANA_INCLUDE_DIRS})
add_executable(example example.cpp)

target_link_libraries(example dana)
