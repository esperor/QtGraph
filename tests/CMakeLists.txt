project(QtGraph)

include(FetchContent)

FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

FetchContent_GetProperties(googletest)

if(NOT googletest_POPULATED)
    FetchContent_Populate(googletest)
    add_subdirectory(${googletest_SOURCE_DIR}
        ${googletest_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()

# Define tests
enable_testing()

qt_add_executable(tests 
    main.cpp
    tst_main.cpp
    tst_protobuf.cpp    
)

target_link_libraries(tests PRIVATE
    gtest_main
    QtGraph
)

target_include_directories(tests 
    PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/../include
)

target_compile_options(tests PRIVATE
    "/Zc:__cplusplus"
    "/permissive-"
)

include(GoogleTest)
gtest_discover_tests(tests)

add_custom_command(
    TARGET tests
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/tests/test_files
            ${CMAKE_SOURCE_DIR}/build/tests/test_files              
)

add_custom_command(
    TARGET tests
    COMMAND ${CMAKE_COMMAND} -E copy -t 
    $<TARGET_FILE_DIR:tests> $<TARGET_RUNTIME_DLLS:tests>
    COMMAND_EXPAND_LISTS
    DEPENDS tests
)