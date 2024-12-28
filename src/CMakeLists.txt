set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(QT_DIR)
  set(CMAKE_PREFIX_PATH ${QT_DIR})
endif()

set(CMAKE_BUILD_TYPE Debug)

find_package(
  Qt6
  COMPONENTS Widgets OpenGLWidgets Core
  REQUIRED)

# file(GLOB TRANS_FILE translation/qpanda_gui_zh_CN.ts)
file(GLOB RESOURCE_FILE resource/resource.qrc)

file(GLOB_RECURSE PROJECT_SOURCES *.cpp *.h)

add_executable(gds_viewer ${PROJECT_SOURCES} ${RESOURCE_FILE}) # ${TRANS_FILE}

target_include_directories(gds_viewer PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(gds_viewer PRIVATE Qt6::Widgets Qt6::OpenGLWidgets
                                         Qt6::Core gdstk)
