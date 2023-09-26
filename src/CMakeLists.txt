project(QtGraph)

add_subdirectory(proto)

set(SOURCES 
    DataClasses/nodespawndata.cpp
    GraphWidgets/Abstracts/abstractpin.cpp
    GraphWidgets/Abstracts/basenode.cpp
    NodeFactoryModule/nfbuttonminimize.cpp
    NodeFactoryModule/nodefactory.cpp
    NodeFactoryModule/typebrowser.cpp
    GraphWidgets/canvas.cpp
    NodeFactoryModule/typednodeimage.cpp
    TypeManagers/nodetypemanager.cpp
    GraphWidgets/pin.cpp
    DataClasses/pindata.cpp
    DataClasses/pindragsignal.cpp
    TypeManagers/pintypemanager.cpp
    GraphWidgets/typednode.cpp
    TypeManagers/typemanager.cpp
    utility.cpp
    idgenerator.cpp
)

set(MOC_HEADERS
    ../include/QtGraph/NodeFactoryModule/nodefactory.h
    ../include/QtGraph/GraphWidgets/Abstracts/abstractpin.h
    ../include/QtGraph/GraphWidgets/Abstracts/basenode.h
    ../include/QtGraph/NodeFactoryModule/nfbuttonminimize.h
    ../include/QtGraph/NodeFactoryModule/typebrowser.h
    ../include/QtGraph/GraphWidgets/canvas.h
    ../include/QtGraph/NodeFactoryModule/typednodeimage.h
    ../include/QtGraph/GraphWidgets/pin.h
    ../include/QtGraph/GraphWidgets/typednode.h
    ../include/QtGraph/TypeManagers/typemanager.h
)

qt_add_library(QtGraph
    STATIC
    ${SOURCES}
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
    ${CMAKE_SOURCE_DIR}/include/QtGraph
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/src/proto
    ${Protobuf_INCLUDE_DIRS}
)

target_include_directories(QtGraph INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

if (CREATE_DLL)
    target_compile_definitions(QtGraph PRIVATE GRAPHLIB_LIBRARY)
    target_compile_definitions(QtGraph INTERFACE IMPORT_GRAPHLIB)
endif()

# For the VS
target_compile_options(QtGraph PRIVATE
    "/Zc:__cplusplus"
    "/permissive-"
)
