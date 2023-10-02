project(QtGraph)

add_subdirectory(proto)

set(SOURCES
    helpers/nodefactory.cpp

    logics/nodetypemanager.cpp
    logics/typemanager.cpp
    logics/pintypemanager.cpp
    logics/pin.cpp
    logics/graph.cpp
    logics/node.cpp

    models/nodespawndata.cpp
    models/pindata.cpp
    models/pindragsignal.cpp

    widgets/pin.cpp
    widgets/node.cpp
    widgets/nfbuttonminimize.cpp
    widgets/typebrowser.cpp
    widgets/canvas.cpp
    widgets/typednodeimage.cpp
    
    utilities/utility.cpp
    utilities/idgenerator.cpp
)

qt_add_library(QtGraph
    STATIC
    ${SOURCES}
    ${QTGRAPH_HEADERS}
    $<TARGET_OBJECTS:protos>
)

set_property(TARGET QtGraph PROPERTY
    MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

target_link_libraries(QtGraph
    PUBLIC
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    protos
    protobuf::libprotobuf
    protobuf::libprotoc
)

target_include_directories(QtGraph PRIVATE
    ${CMAKE_SOURCE_DIR}/include/private
    ${CMAKE_SOURCE_DIR}/include/QtGraph
    ${CMAKE_SOURCE_DIR}/src/proto
    ${Protobuf_INCLUDE_DIRS}
)

target_include_directories(QtGraph INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

# For the VS
target_compile_options(QtGraph PRIVATE
    "/Zc:__cplusplus"
    "/permissive-"
)
