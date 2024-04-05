#cmake

cmake_minimum_required(VERSION 3.10)
project(Server)


find_package(Threads REQUIRED)

add_executable(${PROJECT_NAME} server.cpp)

target_link_libraries(${PROJECT_NAME} PRIVATE Threads::Threads)
