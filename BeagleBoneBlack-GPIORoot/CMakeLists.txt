cmake_minimum_required(VERSION 3.1)
set(PROJECT_NAME BeagleBoneBlack-GPIO)
project(${PROJECT_NAME} CXX)
set(CMAKE_CXX_STANDARD 14)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

set (SOURCE_ROOT BeagleBoneBlack-GPIO/GPIO/)

set(${PROJECT_NAME}_SOURCE_FILES
    ${SOURCE_ROOT}/../BeagleBoneBlack-GPIO.cpp
    ${SOURCE_ROOT}/GPIOConst.cpp
    ${SOURCE_ROOT}/GPIOManager.cpp)

set (${PROJECT_NAME}_HEADER_FILES
    ${SOURCE_ROOT}/GPIOConst.h
    ${SOURCE_ROOT}/GPIOManager.h)

add_library(${PROJECT_NAME}
        ${${PROJECT_NAME}_SOURCE_FILES}
        ${${PROJECT_NAME}_HEADER_FILES})
