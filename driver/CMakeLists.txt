cmake_minimum_required(VERSION 2.8)
project( driver )

set(LIB_NAME "${PROJECT_NAME}")

file(GLOB SOURCES *.c *.cc *.cpp) #add all c/cpp files in SOURCES
file(GLOB HEADERS *.h *.hpp) #add all c/cpp files in HEADERS

# message("Sources: " ${SOURCES})

# add_library(${LIB_NAME} STATIC ${SOURCES} ${HEADER_FILES})
add_library(${LIB_NAME} SHARED ${SOURCES} ${HEADER_FILES})

set_target_properties(
	${LIB_NAME}
	PROPERTIES
	POSITION_INDEPENDENT_CODE ON
)

target_link_libraries(
    ${LIB_NAME}
    pthread
    boost_system
    boost_filesystem
    boost_thread
    boost_serialization
    eml
    ethercat_hardware
)