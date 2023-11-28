cmake_minimum_required(VERSION 3.21)
project(OSLab01)

set(CMAKE_CXX_STANDARD 23)

add_executable(OSLab01
        oslab2_linux_client.cpp
        oslab2_linux_server.cpp
)
target_link_libraries(OSLab01 PRIVATE pthread)