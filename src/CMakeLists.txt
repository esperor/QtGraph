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

#
# Gotta copy the QtGraph.dll to the tests folder
#

# target_compile_definitions(QtGraph PRIVATE
#     "_CRT_SECURE_NO_WARNINGS"
#     "GLFW_INCLUDE_NONE"

#     PUBLIC
#     $<$<CONFIG:Debug>:OE_DEBUG>
#     $<$<CONFIG:Release>:OE_RELEASE>

#     $<$<CONFIG:RelWithDebInfo>:OE_DEBUG>
#     $<$<CONFIG:MinSizeRel>:OE_RELEASE>
# )

# set_property(TARGET QtGraph PROPERTY
#     DLL_PATH 
#     $<IF: 
#         $<OR: $<CONFIG:Debug>, $<CONFIG:RelWithDebInfo> >, 
#     ${CMAKE_SOURCE_DIR}/build/src/Debug, 
#     ${CMAKE_SOURCE_DIR}/build/src/Debug>
# )

# add_custom_target(genexdebug POST_BUILD COMMAND ${CMAKE_COMMAND} -E echo "$<IF: 
# $<OR: $<$<CONFIG:Debug>:1>, $<$<CONFIG:RelWithDebInfo>:1> >, 
# ${CMAKE_SOURCE_DIR}/build/src/Debug, 
# ${CMAKE_SOURCE_DIR}/build/src/Release>")

# if (CMAKE_GENERATOR MATCHES "Visual Studio")
#     if (OE_DEBUG)
#         set(DLL_PATH ${CMAKE_SOURCE_DIR}/build/src/Debug)
#     elseif(OE_RELEASE)
#         set(DLL_PATH ${CMAKE_SOURCE_DIR}/build/src/Release)
#     endif()
# else()
#     set(DLL_PATH ${CMAKE_SOURCE_DIR}/build/src)
# endif()


add_custom_command(
    TARGET QtGraph POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
            ${CMAKE_SOURCE_DIR}/build/src/QtGraph.dll
            ${CMAKE_SOURCE_DIR}/build/tests/QtGraph.dll
    DEPENDS tests
)