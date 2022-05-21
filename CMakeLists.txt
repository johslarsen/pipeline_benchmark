cmake_minimum_required (VERSION 3.2.0)

project(pipeline_benchmark VERSION 0.1.0 LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_FLAGS "-march=native -mcpu=native -mtune=native")

add_definitions(-Wall)

add_library(pipeline_stages input.cpp append.cpp prepend.cpp join.cpp)

file(GLOB benchmarks bench*/*.cpp)
foreach(benchmark ${benchmarks})
  get_filename_component(dirname ${benchmark} DIRECTORY)
  get_filename_component(parent ${dirname} NAME_WE)
  get_filename_component(basename ${benchmark} NAME_WE)
  set(target ${parent}_${basename})
  add_executable(${target} ${benchmark})
  target_link_libraries(${target} pipeline_stages)
endforeach()
