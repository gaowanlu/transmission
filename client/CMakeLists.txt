cmake_minimum_required(VERSION 3.10.0)
project(client)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -Wall -O2")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Wall -std=c++17 -O2")

find_package(OpenCV REQUIRED)

include_directories(${OpenCV_INCLUDE_DIRS})

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/)

include_directories("${PROJECT_SOURCE}/")
include_directories("${PROJECT_SOURCE}/net/")
include_directories("${PROJECT_SOURCE}/code/")
include_directories("${PROJECT_SOURCE}/msg/")

aux_source_directory(./ ROOT_SOURCE)
aux_source_directory(./net/ NET_SOURCE)
aux_source_directory(./code/ CODE_SOURCE)
aux_source_directory(./msg/ MSG_SOURCE)


add_executable(${PROJECT_NAME} main.cpp ${ROOT_SOURCE} ${NET_SOURCE}
                                        ${CODE_SOURCE} ${MSG_SOURCE})

target_link_libraries(${PROJECT_NAME} pthread ${OpenCV_LIBS})


