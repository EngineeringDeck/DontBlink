cmake_minimum_required(VERSION 3.16)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_OBJCXX_STANDARD 20)
set(CMAKE_OBJCXX_STANDARD_REQUIRED ON)

if (APPLE)
    project(Celeste LANGUAGES CXX OBJCXX)
else()
    project(Celeste LANGUAGES CXX)
endif()

if (WIN32)
    execute_process(COMMAND powershell ${CMAKE_SOURCE_DIR}/obs-bootstrap.ps1)
endif()

if (APPLE)
	execute_process(COMMAND powershell ${CMAKE_SOURCE_DIR}/obs-bootstrap.sh)
endif()

## source
add_library(DontBlink MODULE widgets.h widgets.cpp platform.h platform.cpp main.cpp)

if (WIN32)
    target_sources(DontBlink PRIVATE platform-win32.h platform-win32.cpp)
endif()

if (APPLE)
	target_sources(DontBlink PRIVATE platform-macos.h platform-macos.mm)
endif()

## linking
# OBS
if (APPLE OR WIN32)
    set(OBS_SOURCE_DIR ${CMAKE_SOURCE_DIR}/obs-studio)
	if (APPLE)
		set(OBS_LIB_DIR ${CMAKE_SOURCE_DIR}/obs-studio/build)
	else()
		set(OBS_LIB_DIR ${CMAKE_SOURCE_DIR}/obs-studio/build64)
	endif()

    include_directories(${OBS_SOURCE_DIR}/libobs)
    include_directories(${OBS_SOURCE_DIR}/UI/obs-frontend-api/)
    target_include_directories(DontBlink SYSTEM PRIVATE ${OBS_INCLUDE_DIR}/obs)

	find_library(LIBOBS_LIBRARY obs
        ${OBS_LIB_DIR}
        ${OBS_LIB_DIR}/libobs/
        ${OBS_LIB_DIR}/libobs/RelWithDebInfo/
        ${OBS_LIB_DIR}/libobs/Relase/
        ${OBS_LIB_DIR}/libobs/*/
		${OBS_LIB_DIR}/libobs/libobs.framework/
    )
    find_library(OBS_FRONTEND_LIBRARY obs-frontend-api
        ${OBS_LIB_DIR}
        ${OBS_LIB_DIR}/UI/obs-frontend-api/
        ${OBS_LIB_DIR}/UI/obs-frontend-api/RelWithDebInfo/
        ${OBS_LIB_DIR}/UI/obs-frontend-api/Release/
        ${OBS_LIB_DIR}/UI/obs-frontend-api/*/
    )

    target_link_libraries(DontBlink ${OBS_LIB_DIR}/libobs/libobs.dylib ${OBS_FRONTEND_LIBRARY})
endif()

# Apple Frameworks
if (APPLE)
	find_library(FOUNDATION Foundation)
	find_library(APPKIT AppKit)
	target_link_libraries(DontBlink ${FOUNDATION} ${APPKIT})
endif()

# Qt
find_package(Qt5 COMPONENTS Widgets REQUIRED)
target_link_libraries(DontBlink Qt5::Widgets)





