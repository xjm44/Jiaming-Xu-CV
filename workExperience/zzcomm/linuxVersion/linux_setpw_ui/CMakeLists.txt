cmake_minimum_required(VERSION 3.5)

project(setpw LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_BUILD_TYPE "Release")
# set(CMAKE_BUILD_TYPE "Debug")
# set(CMAKE_BUILD_TYPE "RelWithDebInfo")

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS_RELEASE "-s -O3")
set(CMAKE_CXX_FLAGS_DEBUG  "-g -O0")
set(CMAKE_CXX_FLAGS "-Wall -pthread") #-fcommon -std=c++14 

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
#set(CMAKE_AUTOUIC_SEARCH_PATHS ./src/ui/ui_gui)
list(APPEND CMAKE_AUTOUIC_SEARCH_PATHS "./")

find_package(Qt5 COMPONENTS Core Widgets REQUIRED)

aux_source_directory(. srcs)


file(GLOB all_headers 
${CMAKE_SOURCE_DIR}/*.hpp 
${CMAKE_SOURCE_DIR}/*.h 
)

message(NOTICE "main: ${srcs}")
message(NOTICE " include: ${all_headers}")
#message(NOTICE "uis: ${all_uis}")

link_directories(
)

add_executable(${PROJECT_NAME}
	#WIN32 # If you need a terminal for debug, please comment this statement 
	${all_headers} 
	${srcs} 
)

target_link_libraries(${PROJECT_NAME} PRIVATE Qt5::Widgets)


