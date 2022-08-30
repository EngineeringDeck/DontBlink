cmake_minimum_required(VERSION 3.16)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (APPLE)
    project(Celeste LANGUAGES CXX OBJCXX)
else()
    project(Celeste LANGUAGES CXX)
endif()

if (WIN32)
    execute_process(COMMAND powershell ${CMAKE_SOURCE_DIR}/obs-bootstrap.ps1)
endif()

find_package(Qt5 COMPONENTS Widgets REQUIRED)
add_library(DontBlink MODULE widgets.h widgets.cpp platform.h platform.cpp main.cpp)

if (WIN32)
    target_sources(DontBlink PRIVATE platform-win32.h platform-win32.cpp)

    set(OBS_SOURCE_DIR ${CMAKE_SOURCE_DIR}/obs-studio)
    set(OBS_LIB_DIR ${CMAKE_SOURCE_DIR}/obs-studio/build64)

    include_directories(${OBS_SOURCE_DIR}/libobs)
    include_directories(${OBS_SOURCE_DIR}/UI/obs-frontend-api/)
    target_include_directories(DontBlink SYSTEM PRIVATE ${OBS_INCLUDE_DIR}/obs)

    find_library(LIBOBS_LIBRARY obs
        ${OBS_LIB_DIR}
        ${OBS_LIB_DIR}/libobs/
        ${OBS_LIB_DIR}/libobs/RelWithDebInfo/
        ${OBS_LIB_DIR}/libobs/Relase/
        ${OBS_LIB_DIR}/libobs/*/
    )
    find_library(OBS_FRONTEND_LIBRARY obs-frontend-api
        ${OBS_LIB_DIR}
        ${OBS_LIB_DIR}/UI/obs-frontend-api/
        ${OBS_LIB_DIR}/UI/obs-frontend-api/RelWithDebInfo/
        ${OBS_LIB_DIR}/UI/obs-frontend-api/Release/
        ${OBS_LIB_DIR}/UI/obs-frontend-api/*/
    )

    target_link_libraries(DontBlink ${LIBOBS_LIBRARY} ${OBS_FRONTEND_LIBRARY} Qt5::Widgets)
endif()








