file(GLOB_RECURSE GDSTK_SOURCE *.cpp *.c)
find_package(ZLIB)
add_library(gdstk STATIC ${GDSTK_SOURCE})

target_include_directories(gdstk PRIVATE ${ZLIB_INCLUDE_DIRS} "./libqhull_r" "./clipperlib")
target_link_libraries(gdstk ${ZLIB_LIBRARIES})

set_target_properties(
  gdstk PROPERTIES INTERFACE_INCLUDE_DIRECTORIES
                   "${CMAKE_CURRENT_SOURCE_DIR}/..;${CMAKE_CURRENT_SOURCE_DIR}")
