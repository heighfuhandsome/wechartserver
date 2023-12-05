cmake_minimum_required(VERSION 3.0.0)

project(myServer)

set(CMAKE_CXX_STANDARD 17)

#set(CMAKE_BUILD_TYPE debug)

add_compile_options(-Wall -g)

add_subdirectory(${PROJECT_SOURCE_DIR}/server)


