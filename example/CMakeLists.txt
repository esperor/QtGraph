project(QtGraph)

set(SOURCES main.cpp mainwindow.cpp)
set(HEADERS mainwindow.h)

qt_wrap_ui(SOURCES mainwindow.ui)

qt_add_executable(QtGraphExample
    ${SOURCES}
    ${HEADERS}
)

target_link_libraries(QtGraphExample PRIVATE
    QtGraph
)

target_include_directories(QtGraphExample 
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../include/
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# For the VS
target_compile_options(QtGraphExample PRIVATE 
    "/Zc:__cplusplus"
    "/permissive-"
)

set_target_properties(QtGraphExample PROPERTIES
    WIN32_EXECUTABLE ON
    MACOSX_BUNDLE ON
)

add_custom_command(
    TARGET QtGraphExample POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy -t 
    $<TARGET_FILE_DIR:QtGraphExample> $<TARGET_RUNTIME_DLLS:QtGraphExample>
    COMMAND_EXPAND_LISTS
)
