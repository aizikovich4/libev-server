project(async-server-libeb)

cmake_minimum_required(VERSION 3.10)

project(server)

set(CMAKE_CXX_STANDARD 17)

FILE (GLOB SOURCE_FILES
    ./*.cpp
    ./*.hpp
    )

add_executable(${PROJECT_NAME} ${SOURCE_FILES} )
target_link_libraries(${PROJECT_NAME} ev pthread)

