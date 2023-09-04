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

# add_custom_command(
#     TARGET QtGraph POST_BUILD
#     COMMAND ${CMAKE_COMMAND} -E copy
#             ${CMAKE_SOURCE_DIR}/build/src/QtGraph.dll
#             ${CMAKE_SOURCE_DIR}/build/tests/QtGraph.dll
#     DEPENDS QtGraph-
# )

# protoc pin.proto -I ../src/proto -I D:/github/google/googleapis --cpp_out=../src/proto/

# qt_add_protobuf(DEPENDENCIES 
#     GENERATE_PACKAGE_SUBFOLDERS
#     PROTO_FILES
#         ${PROTO_WELL_KNOWN}/google/type/color.proto
#     OUTPUT_DIRECTORY
#         ${CMAKE_SOURCE_DIR}/protos_generated
# )

# target_include_directories(DEPENDENCIES
#     PRIVATE
#     ${PROTOBUF_SRC}
#     D:\\Github\\abseil-cpp\\Source\\abseil-cpp
# )

# qt_add_protobuf(protos
#     GENERATE_PACKAGE_SUBFOLDERS
#     EXTRA_NAMESPACE Protocol
#     PROTO_FILES
#         proto/state.proto
#         proto/pin.proto
#         proto/type/color.proto
#         proto/type/point.proto
#         proto/node.proto
#     PROTO_INCLUDES
#         ${CMAKE_SOURCE_DIR}/src/proto
#     OUTPUT_DIRECTORY
#         ${CMAKE_SOURCE_DIR}/protos_generated
#     OUTPUT_TARGETS PROTO_SRCS
# )

# protobuf_generate(PROTO_SRCS PROTO_HDRS 
#     proto/pin.proto
#     PROTOC_OUT_DIR 
#         ${CMAKE_SOURCE_DIR}/protos_generated
# )
# protobuf_generate_python(PROTO_PY proto/pin.proto)

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
    #${Protobuf_LIBRARIES}
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


# add_custom_command(
#     TARGET QtGraph POST_BUILD
#     COMMAND ${CMAKE_COMMAND} -E copy
#             ${CMAKE_SOURCE_DIR}/build/src/QtGraph.dll
#             ${CMAKE_SOURCE_DIR}/build/tests/QtGraph.dll
#     DEPENDS tests
# )