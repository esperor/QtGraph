project(QtGraph)

set(SOURCES 
    DataClasses/nodespawndata.cpp
    GraphWidgets/Abstracts/abstractpin.cpp
    GraphWidgets/Abstracts/basenode.cpp
    NodeFactoryModule/nfbuttonminimize.cpp
    NodeFactoryModule/nodefactory.cpp
    NodeFactoryModule/nodefactorywidget.cpp
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
)

set(MOC_HEADERS
    ../include/QtGraph/NodeFactoryModule/nodefactory.h
    ../include/QtGraph/GraphWidgets/Abstracts/abstractpin.h
    ../include/QtGraph/GraphWidgets/Abstracts/basenode.h
    ../include/QtGraph/NodeFactoryModule/nfbuttonminimize.h
    ../include/QtGraph/NodeFactoryModule/nodefactorywidget.h
    ../include/QtGraph/GraphWidgets/canvas.h
    ../include/QtGraph/NodeFactoryModule/typednodeimage.h
    ../include/QtGraph/GraphWidgets/pin.h
    ../include/QtGraph/GraphWidgets/typednode.h
    ../include/QtGraph/TypeManagers/typemanager.h
)

# qt_wrap_cpp(SOURCES ${MOC_HEADERS})

qt_add_library(QtGraph
    SHARED
    ${SOURCES}
)

target_link_libraries(QtGraph
    PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
)

target_include_directories(QtGraph PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../include/QtGraph
    ${CMAKE_CURRENT_SOURCE_DIR}/../include
)

target_include_directories(QtGraph INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

target_compile_definitions(QtGraph PRIVATE GRAPHLIB_LIBRARY)

# For the VS
target_compile_options(QtGraph PRIVATE
    "/Zc:__cplusplus"
    "/permissive-"
)

add_custom_command(
        TARGET QtGraph POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
                ${CMAKE_SOURCE_DIR}/build/src/QtGraph.dll
                ${CMAKE_SOURCE_DIR}/build/tests/QtGraph.dll)