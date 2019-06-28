cmake_minimum_required(VERSION 3.14)
project(LightningWord)
set(CMAKE_CXX_STANDARD 11)

aux_source_directory(. DIR_SRCS)

add_subdirectory(system)

add_executable(LightningWord main.cpp ${DIR_SRCS})

target_link_libraries(LightningWord system)