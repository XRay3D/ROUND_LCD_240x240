#/*******************************************************************************
#* Author    :  Damir Bakiev                                                    *
#* Version   :  na                                                              *
#* Date      :  11 November 2021                                                *
#* Website   :  na                                                              *
#* Copyright :  Damir Bakiev 2016-2022                                          *
#* License:                                                                     *
#* Use, modification & distribution is subject to Boost Software License Ver 1. *
#* http://www.boost.org/LICENSE_1_0.txt                                         *
#*******************************************************************************/
cmake_minimum_required(VERSION 3.16)

#set(PROJECT_NAME "GGEasy")

#project(${PROJECT_NAME} LANGUAGES CXX)
project(lvgl_test)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
#set(CMAKE_CXX_EXTENSIONS OFF)
#set(PREBUILT_MODULE_PATH ${CMAKE_BINARY_DIR}/modules)

#add_subdirectory(${CMAKE_SOURCE_DIR}/../lvgl/env_support/cmake/scustom.cmake)
set(LVGL_ROOT_DIR ../lvgl)
include(../lvgl/env_support/cmake/custom.cmake)
include_directories(${LVGL_ROOT_DIR}/src)


#set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${OUTPUT_DIRECTORY})
#set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${OUTPUT_DIRECTORY})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${OUTPUT_DIRECTORY})

string(TIMESTAMP DT "%d.%m.%Y - %H:%M:%S")# [<format string>] [UTC])
add_definitions(
    -DBUILD_DATE="${DT}"
    -DLV_MEM_CUSTOM=1
    -DLV_LVGL_H_INCLUDE_SIMPLE=1
    )

find_package(QT NAMES Qt6 Qt5 COMPONENTS Core Gui Widgets REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS
    #    Concurrent
    Core
    Gui
    LinguistTools
    OpenGL
    PrintSupport
    Widgets
    REQUIRED
    )

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    find_package(Qt${QT_VERSION_MAJOR} COMPONENTS OpenGLWidgets REQUIRED)
endif()

include_directories (
    ${INCLUDE_DIRECTORIES}
    )

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_create_translation(QM_FILES ${CMAKE_CURRENT_SOURCE_DIR}/translations ${TS_FILES})
#    qt6_add_resources(RESOURCES res/resources.qrc)
else()
    qt5_create_translation(QM_FILES ${CMAKE_CURRENT_SOURCE_DIR}/translations ${TS_FILES})
#    qt5_add_resources(RESOURCES res/resources.qrc)
endif()

#set(APP_ICON_RESOURCE_WINDOWS "${CMAKE_CURRENT_SOURCE_DIR}/myapp.rc")


file(GLOB_RECURSE SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp ${CMAKE_CURRENT_SOURCE_DIR}/*.c)
file(GLOB_RECURSE HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/*.h)
file(GLOB_RECURSE UI ${CMAKE_CURRENT_SOURCE_DIR}/*.ui)

add_executable(${PROJECT_NAME}
    ${SOURCES}
    ${DEMO_SOURCES}
    ${de}
    ${TS_FILES}
    ${SOURCE}
    ${HEADERS}
    ${UI}
    ${RESOURCES}
#    ${APP_ICON_RESOURCE_WINDOWS}
    ${MAGIC_GET}
    )

target_link_libraries(${PROJECT_NAME} PRIVATE
#    clipper
#    common
#    filetree
#    gi
#    graphicsview
#    shape
#    thermal
#    tooldatabase
    #    Qt${QT_VERSION_MAJOR}::Concurrent
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Gui
    Qt${QT_VERSION_MAJOR}::OpenGL
    Qt${QT_VERSION_MAJOR}::PrintSupport
    Qt${QT_VERSION_MAJOR}::Widgets
    )

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    target_link_libraries(${PROJECT_NAME} PRIVATE Qt${QT_VERSION_MAJOR}::OpenGLWidgets)
endif()

install(TARGETS ${PROJECT_NAME} DESTINATION ${OUTPUT_DIRECTORY})

if(WIN32)
    target_link_libraries(${PROJECT_NAME} PRIVATE setupapi)
    # windeployqt
    set(CMAKE_INSTALL_UCRT_LIBRARIES TRUE)
    get_target_property(_QMAKE_EXECUTABLE Qt${QT_VERSION_MAJOR}::qmake IMPORTED_LOCATION)
    get_filename_component(_QT_BIN_DIR "${_QMAKE_EXECUTABLE}" DIRECTORY)
    find_program(WINDEPLOYQT_EXECUTABLE windeployqt HINTS "${_QT_BIN_DIR}")
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND "${CMAKE_COMMAND}" -E
        env PATH="${_QT_BIN_DIR}" "${WINDEPLOYQT_EXECUTABLE}"
        --verbose 0
        --no-compiler-runtime
        \"$<TARGET_FILE:${PROJECT_NAME}>\"
        COMMENT "Deploying Qt..."
        )
elseif (UNIX)#LINUX
    #    cqtdeployer -bin myApp -qmake path/to/Qt/5.x.x/build/bin/qmake -qmlDir path/to/my/qml/files/dir qif
endif()

#execute_process(
#    COMMAND "${CMAKE_COMMAND}" -E
#        env PATH="${_qt_bin_dir}" "${WINDEPLOYQT_EXECUTABLE}"
#            --dry-run
#            --no-compiler-runtime
#            --list mapping
#            \"$<TARGET_FILE:myapp>\"
#    OUTPUT_VARIABLE _output
#    OUTPUT_STRIP_TRAILING_WHITESPACE
#)

set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "${PROJECT_NAME}${SUFFIX}")

if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        LINK_FLAGS_DEBUG "/SUBSYSTEM:CONSOLE"
        LINK_FLAGS_RELWITHDEBINFO "/SUBSYSTEM:CONSOLE"
        LINK_FLAGS_RELEASE "/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup"
        LINK_FLAGS_MINSIZEREL "/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup"
        )
endif()


set(TARGETS
    ${PROJECT_NAME}
    )

foreach(TNAME ${TARGETS})
    target_compile_features(${TNAME} PUBLIC  cxx_std_20)
    #  target_include_directories(${TNAME} PUBLIC  ${CMAKE_CURRENT_BINARY_DIR})
    #  target_include_directories(${TNAME} PUBLIC  ${CMAKE_CURRENT_SOURCE_DIR})
endforeach()
