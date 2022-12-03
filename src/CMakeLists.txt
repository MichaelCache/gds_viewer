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
  QT NAMES Qt5
  COMPONENTS Widgets LinguistTools
  REQUIRED)
find_package(
  Qt${QT_VERSION_MAJOR}
  COMPONENTS Widgets LinguistTools
  REQUIRED)

# file(GLOB TRANS_FILE translation/qpanda_gui_zh_CN.ts)
file(GLOB RESOURCE_FILE resource/resource.qrc)

file(GLOB_RECURSE PROJECT_SOURCES *.cpp *.h)

add_executable(QEDA ${PROJECT_SOURCES} ${RESOURCE_FILE}) # ${TRANS_FILE} 

# qt5_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TRANS_FILE})

# find_package(gdstk REQUIRED)
target_include_directories(QEDA INTERFACE gdstk)
# target_link_libraries(QEDA gdstk_LIBRARIES)

target_link_libraries(QEDA PRIVATE Qt${QT_VERSION_MAJOR}::Widgets gdstk)

set_target_properties(
  QEDA
  PROPERTIES MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
             MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
             MACOSX_BUNDLE_SHORT_VERSION_STRING
             ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR})
