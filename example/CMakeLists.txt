project(QtGraph)

set(SOURCES main.cpp mainwindow.cpp)

qt_wrap_ui(SOURCES mainwindow.ui)

qt_add_executable(QtGraphExample
    ${SOURCES}
)

target_link_libraries(QtGraphExample PRIVATE
    QtGraph
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
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