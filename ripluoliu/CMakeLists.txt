cmake_minimum_required(VERSION 3.20 FATAL_ERROR)

project(cctv-dev)

### Dependencies #############################################################

add_subdirectory(../3rdparty/csUtil/csUtil
  ${CMAKE_CURRENT_BINARY_DIR}/csUtil
)

### Project ##################################################################

list(APPEND ripluoliu_HEADERS
  include/block.h
  include/fourcc.h
  include/toc.h
  include/util.h
)

list(APPEND ripluoliu_SOURCES
  src/block.cpp
  src/fourcc.cpp
  src/toc.cpp
  src/util.cpp
  src/main.cpp
)

### Target ###################################################################

add_executable(ripluoliu)

set_target_properties(ripluoliu PROPERTIES
  CXX_STANDARD 20
  CXX_STANDARD_REQUIRED ON
)

target_include_directories(ripluoliu
  PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(ripluoliu
  PRIVATE csUtil
)

target_sources(ripluoliu
  PRIVATE ${ripluoliu_HEADERS}
  PRIVATE ${ripluoliu_SOURCES}
)
