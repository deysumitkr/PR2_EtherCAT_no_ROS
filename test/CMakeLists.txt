cmake_minimum_required(VERSION 2.8)
project( test_driver )

set(EXE_NAME "${PROJECT_NAME}")

file(GLOB SOURCES *.c *.cc *.cpp) #add all c/cpp files in SOURCES
file(GLOB HEADERS *.h *.hpp) #add all c/cpp files in HEADERS

# message("Sources: " ${SOURCES})
# message("Headers: " ${HEADERS})

add_executable(${EXE_NAME} ${SOURCES} ${HEADERS})

set_target_properties(
	${EXE_NAME}
	PROPERTIES
	POSITION_INDEPENDENT_CODE ON
)

target_link_libraries(
    ${EXE_NAME}
    pthread
    boost_system
    boost_filesystem
    boost_thread
    boost_serialization
    eml
    driver
    # ethercat_hardware
)