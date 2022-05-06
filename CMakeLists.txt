cmake_minimum_required(VERSION 3.15)
project(dbf)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "-g")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE INTERNAL "")

file(GLOB HEADER_FILE ${CMAKE_SOURCE_DIR}/header/*.h)
file(GLOB SOURCE_FILE ${CMAKE_SOURCE_DIR}/src/*.cpp)

add_executable(${PROJECT_NAME}
                ${HEADER_FILE}
                ${SOURCE_FILE}
                main.cpp)
