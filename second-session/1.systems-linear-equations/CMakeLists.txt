cmake_minimum_required(VERSION 2.4)

project(sys_lineq)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set(CMAKE_SHARED_LINKER_FLAGS "-lgsl -lgslcblas -lm")
set(CMAKE_EXE_LINKER_FLAGS "-lgsl -lgslcblas -lm")

set(SOURCE_FILES main.cpp functions.cpp)
add_executable(sys_lineq.bin ${SOURCE_FILES})
target_link_libraries(sys_lineq.bin -lgsl -lgslcblas -lm)

MAKE_DIRECTORY(output)