cmake_minimum_required(VERSION 3.5)

project(mainApp LANGUAGES CXX)

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
list(APPEND CMAKE_AUTOUIC_SEARCH_PATHS "./src/ui/ui_gui" "./src/ui/ui_dialog" "./src/ui/ui_widget")

find_package(Qt5 COMPONENTS Core Widgets REQUIRED)
# find_package(Qt5 COMPONENTS Core Widgets LinguistTools REQUIRED)
find_package(yaml-cpp REQUIRED)

aux_source_directory(./src srcs)
aux_source_directory(./src/control_authentity aut)
aux_source_directory(./src/dialog/dialog_gui dia)
aux_source_directory(./src/extern exts)
aux_source_directory(./src/gui m)
aux_source_directory(./src/tools/process process)
aux_source_directory(./src/widgets/widget_gui w_gui)
aux_source_directory(./resources/language lang)


file(GLOB all_headers 
${CMAKE_SOURCE_DIR}/src/control_authentity/*.hpp 
${CMAKE_SOURCE_DIR}/src/control_authentity/*.h 
${CMAKE_SOURCE_DIR}/src/include/*/*.h 
${CMAKE_SOURCE_DIR}/resources/language/linguist.h
)




# # for translation
# set(TS_FILES)
# set(QM_FILES)
# list(APPEND TS_FILES a.ts)
# set_source_files_properties(${TS_FILES}
#     PROPERTIES OUTPUT_LOCATION .
# )
# qt5_create_translation(QM_FILES ${TS_FILES} ./src/main.cpp)

# add_custom_target(lupdate_task ALL DEPENDS ${TS_FILES})
# add_custom_target(lrelease_task ALL DEPENDS ${QM_FILES})



#message(NOTICE "main: ${srcs}")
#message(NOTICE " include: ${all_headers}")
#message(NOTICE "uis: ${all_uis}")

link_directories(
)

include_directories(${YAML_CPP_INCLUDE_DIR})

add_executable(${PROJECT_NAME}
	#WIN32 # If you need a terminal for debug, please comment this statement 

	${all_headers} 
	${srcs} 
	${exts} 
	${aut} 
	${dia} 
	${m} 
	${process}
	${w_gui}  
	${lang}
	${CMAKE_SOURCE_DIR}/resources/qrc/mainApp.qrc
	# ${QM_FILES}
)

target_link_libraries(${PROJECT_NAME} PRIVATE Qt5::Widgets libyaml-cpp.so) 


