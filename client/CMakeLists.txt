#cmake

cmake_minimum_required(VERSION 3.10)
project(Client)


find_package(Threads REQUIRED)

add_executable(${PROJECT_NAME} client.cpp)

target_link_libraries(${PROJECT_NAME} PRIVATE Threads::Threads)
