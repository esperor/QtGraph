project(QtGraph)

file(MAKE_DIRECTORY ${CMAKE_SOURCE_DIR}/protos_generated)

add_library(protos OBJECT 
    state.proto
    node.proto
    pin.proto
    type/color.proto
    type/point.proto
    typemanager.proto
    graph.proto
    structure.proto
)

set(PROTO_BINARY_DIR ${CMAKE_SOURCE_DIR}/protos_generated)

set_property(TARGET protos PROPERTY
    MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

target_link_libraries(protos PUBLIC protobuf::libprotobuf)

target_include_directories(protos PUBLIC $<BUILD_INTERFACE:${PROTO_BINARY_DIR}>)

protobuf_generate(
    TARGET protos
    LANGUAGE cpp
    IMPORT_DIRS ${CMAKE_CURRENT_SOURCE_DIR}
    PROTOC_OUT_DIR ${PROTO_BINARY_DIR}
)