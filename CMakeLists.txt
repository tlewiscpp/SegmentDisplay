cmake_minimum_required(VERSION 3.1)
set(PROJECT_NAME SegmentDisplay)
project(${PROJECT_NAME} CXX)
set(CMAKE_CXX_STANDARD 14)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

add_subdirectory(BeagleBoneBlack-GPIO/)

set (SOURCE_ROOT src/)

set(${PROJECT_NAME}_SOURCE_FILES
        ${SOURCE_ROOT}/Main.cpp
        ${SOURCE_ROOT}/ApplicationUtilities.cpp
        ${SOURCE_ROOT}/MessageLogger.cpp
        ${SOURCE_ROOT}/SegmentDisplay.cpp)

set (${PROJECT_NAME}_HEADER_FILES
        ${SOURCE_ROOT}/ApplicationUtilities.h
        ${SOURCE_ROOT}/MessageLogger.h
        ${SOURCE_ROOT}/GlobalDefinitions.h
        ${SOURCE_ROOT}/SegmentDisplay.h)

add_executable(${PROJECT_NAME}
        ${${PROJECT_NAME}_SOURCE_FILES}
        ${${PROJECT_NAME}_HEADER_FILES})

target_link_libraries(${PROJECT_NAME} pthread)
