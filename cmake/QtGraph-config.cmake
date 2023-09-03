
get_filename_component(SELF_DIR ${CMAKE_CURRENT_LIST_DIR} PATH)

include(CMakeFindDependencyMacro)

find_dependency(Qt6 REQUIRED COMPONENTS Core Gui Widgets)
find_dependency(protobuf REQUIRED CONFIG)
include(${SELF_DIR}/cmake/QtGraph.cmake)