cmake_minimum_required(VERSION 3.20 FATAL_ERROR)

project(cctv-dev)

### Dependencies #############################################################

### Project ##################################################################

list(APPEND ripluoliu_SOURCES
  src/main.cpp
)

### Target ###################################################################

add_executable(ripluoliu)

set_target_properties(ripluoliu PROPERTIES
  CXX_STANDARD 20
  CXX_STANDARD_REQUIRED ON
)

target_sources(ripluoliu
  PRIVATE ${ripluoliu_SOURCES}
)
