cmake_minimum_required(VERSION 3.1)
project(azs_local_server)

add_subdirectory(Crow)
link_directories(Crow/include)
add_executable(${PROJECT_NAME} main.cpp)
target_link_libraries(${PROJECT_NAME} Crow)