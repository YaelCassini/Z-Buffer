cmake_minimum_required(VERSION 3.15)

# set the project name
project(Z-Buffer VERSION 1.0.0)

file(GLOB_RECURSE ALL_SOURCE   
${PROJECT_SOURCE_DIR}/src/*.c
${PROJECT_SOURCE_DIR}/src/*.cpp
)

include_directories(${PROJECT_SOURCE_DIR}/src)

# add the executable
add_executable(${PROJECT_NAME} ${ALL_SOURCE})