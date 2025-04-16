cmake_minimum_required(VERSION 3.20)
project(shm_demo_cpp LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 14)

if (NOT DEFINED OpenCV_DIR)
    # Get OpenCV from system environment
    if (DEFINED ENV{OpenCV_INSTALL_PATH})
        set(OpenCV_DIR $ENV{OpenCV_INSTALL_PATH})
    endif ()
endif ()
find_package(OpenCV REQUIRED)
if (DEFINED OpenCV_LIBS AND DEFINED OpenCV_INCLUDE_DIRS)
    set(OpenCV_FOUND TRUE)
    include_directories(${OpenCV_INCLUDE_DIRS})
else ()
    set(OpenCV_FOUND FALSE)
endif ()

add_executable(${PROJECT_NAME} main.cpp)

target_sources(${PROJECT_NAME}
        PRIVATE
        main.cpp
        shared_memory_tool.h
)

target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBS})