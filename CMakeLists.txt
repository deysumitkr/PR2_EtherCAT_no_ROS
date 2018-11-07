cmake_minimum_required(VERSION 2.4.6)
project(pr2_EtherCAT_no_ROS)


set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)
set(CMAKE_DISABLE_SOURCE_CHANGES  ON)

if ("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_BINARY_DIR}")
  message(SEND_ERROR "In-source builds are not allowed.")
endif ()

set(CMAKE_COLOR_MAKEFILE   ON)

set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/build)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})


find_package( Boost REQUIRED COMPONENTS )

set( Boost_USE_STATIC_LIBS ON )
set( Boost_USE_MULTITHREADED ON )

include_directories(
    ${PROJECT_SOURCE_DIR}
    ${PROJECT_SOURCE_DIR}/eml/include
    ${PROJECT_SOURCE_DIR}/ethercat_hardware
    ${Boost_INCLUDE_DIR}
)

link_directories(
    ${PROJECT_SOURCE_DIR}/eml/lib/
    # ${PROJECT_SOURCE_DIR}/ethercat_hardware/lib/
)

add_definitions(
    -Wall
    -pthread
)

add_subdirectory(ethercat_hardware)
add_subdirectory(driver)
add_subdirectory(test)