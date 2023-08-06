cmake_minimum_required(VERSION 3.16)

project(QtGraph)

# Avoid warning about DOWNLOAD_EXTRACT_TIMESTAMP in CMake 3.24:
if (CMAKE_VERSION VERSION_GREATER_EQUAL "3.24.0")
    cmake_policy(SET CMP0135 NEW)
endif()

list(APPEND CMAKE_PREFIX_PATH "d:\\Qt\\6.5.2\\msvc2019_64\\lib\\cmake\\Qt6")

set(Vulkan_INCLUDE_DIR "C:\\VulkanSDK\\1.3.250.1\\Include")
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(AUTOMOC ON)
set(CMAKE_AUTOMOC ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
find_package(Qt6 6.5.2 REQUIRED COMPONENTS Core Gui Widgets)

# qt_standard_project_setup()

add_subdirectory(include)
add_subdirectory(src)
add_subdirectory(thirdparty)

option(BUILD_TESTS "" ON)
option(BUILD_EXAMPLE "" ON)

if (BUILD_TESTS)
    message(STATUS "Configuring tests...")
    add_subdirectory(tests)
endif()

if (BUILD_EXAMPLE)
    message(STATUS "Configuring example...")
    add_subdirectory(example)
endif()