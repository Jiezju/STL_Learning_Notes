cmake_minimum_required(VERSION 3.20)
project(stl_3_0)

set(CMAKE_CXX_STANDARD 11)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

file(GLOB_RECURSE srcs ${CMAKE_CURRENT_SOURCE_DIR}/test/*.cpp)

# 打印变量
#message(${srcs})

foreach(source ${srcs})
#    message(${source})
    get_filename_component(name ${source} NAME_WE)
    # target
    add_executable(${name} ${source})
endforeach(source)