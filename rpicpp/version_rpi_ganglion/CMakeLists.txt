# Version
cmake_minimum_required(VERSION 3.10)

# Name
set(PROJECT_NAME neuroslave)
project(${PROJECT_NAME})

# Tools paths
set(TOOLS_PATH "../../libc")

# Apply settings
include(${NSV_BUILD_FOLDER}/settings.cmake)

# Flags
set(MAIN_FLAGS "-Ofast -Wall -pthread")
set(ALL_CXX_FLAGS "${MAIN_FLAGS} ${PLATFORM_FLAGS}")

# Includes
set(COMMON_TOOLS_PATH "${TOOLS_PATH}/common_tools")
set(COMMUNICATION_PATH "${TOOLS_PATH}/communication/clientTCP")
list(APPEND COMMUNICATION_PATH "${TOOLS_PATH}/communication/serverTCP")
list(APPEND COMMUNICATION_PATH "${TOOLS_PATH}/communication/communicator")
set(GANGLION_COMM_PATH "${TOOLS_PATH}/ganglion_comm/definitions")
set(NEUROSLAVE_TOOLS_PATH "${TOOLS_PATH}/neuroslave")
list(APPEND NEUROSLAVE_TOOLS_PATH "${TOOLS_PATH}/neuroslave/definitions")
list(APPEND NEUROSLAVE_TOOLS_PATH "${TOOLS_PATH}/neuroslave/source")
list(APPEND NEUROSLAVE_TOOLS_PATH "${TOOLS_PATH}/neuroslave/handler")
set(RAPID_JSON_PATH "${TOOLS_PATH}/third_party/rapidjson")
set(FILE_TOOLS_PATH "${TOOLS_PATH}/file_tools")
# Include it!
set(ALL_INCLUDE_PATH "")
list(APPEND ALL_INCLUDE_PATH ${COMMON_TOOLS_PATH})
list(APPEND ALL_INCLUDE_PATH ${COMMUNICATION_PATH})
list(APPEND ALL_INCLUDE_PATH ${GANGLION_COMM_PATH})
list(APPEND ALL_INCLUDE_PATH ${NEUROSLAVE_TOOLS_PATH})
list(APPEND ALL_INCLUDE_PATH ${RAPID_JSON_PATH})
list(APPEND ALL_INCLUDE_PATH ${FILE_TOOLS_PATH})
# Platform dependent includes
list(APPEND ALL_INCLUDE_PATH ${PLATFORM_PATH})

# standards & options
add_compile_options(-Wno-error)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_POSITION_INDEPENDED_CODE ON)
SET(CMAKE_CXX_FLAGS "${ALL_CXX_FLAGS}")

# main cpp
set(MAIN_SOURCE "nsvtest.cpp")

# set source
set(SOURCE_LIST ${MAIN_SOURCE})
FOREACH(INC_PATH ${ALL_INCLUDE_PATH}) 
	file(GLOB_RECURSE LOCAL_SOURCE_LIST_CPP "${INC_PATH}/*.cpp")
	file(GLOB_RECURSE LOCAL_SOURCE_LIST_C "${INC_PATH}/*.c")
	#file(GLOB_RECURSE LOCAL_SOURCE_LIST_H "${INC_PATH}/*.h")
	list(APPEND SOURCE_LIST ${LOCAL_SOURCE_LIST_CPP} ${LOCAL_SOURCE_LIST_C} ${LOCAL_SOURCE_LIST_H})
ENDFOREACH()

# add sources
# message(STATUS "${PROJECT_NAME} sources = ${SOURCE_LIST}")
add_executable(${PROJECT_NAME} ${SOURCE_LIST})

# include & link
target_include_directories(${PROJECT_NAME} PRIVATE ${ALL_INCLUDE_PATH}) # ${ALL_LIBS_PATH})
# target_link_directories(${PROJECT_NAME} PRIVATE ${ALL_LIBS_PATH})
#target_link_libraries(${PROJECT_NAME} PRIVATE ${ALL_LIBS})
