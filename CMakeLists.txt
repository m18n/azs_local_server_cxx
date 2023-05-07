make_minimum_required(VERSION 3.1)
project(my_project)

find_package(crow REQUIRED)

add_executable(my_app main.cpp)

target_link_libraries(my_app crow::crow)