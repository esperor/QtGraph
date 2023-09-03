cmake_minimum_required(VERSION 3.16)

project(QtGraph)

# Avoid warning about DOWNLOAD_EXTRACT_TIMESTAMP in CMake 3.24:
if (CMAKE_VERSION VERSION_GREATER_EQUAL "3.24.0")
    cmake_policy(SET CMP0135 NEW)
endif()

set(CMAKE_VERBOSE_MAKEFILE ON)

## commented due to use of protobuf 21.12
# include(FindProtobuf)

set(Vulkan_INCLUDE_DIR "C:\\VulkanSDK\\1.3.250.1\\Include")

# set(Protobuf_PREFIX_PATH
#     "c:\\Program Files (x86)\\protobuf/include"            
#     "c:\\Program Files (x86)\\protobuf/lib"             
#     "c:\\Program Files (x86)\\protobuf/bin"
#     "c:\\Program Files (x86)\\protobuf/cmake" 
# )
# list(APPEND CMAKE_PREFIX_PATH 
#     ${Protobuf_PREFIX_PATH}
# )



set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(AUTOMOC ON)
set(CMAKE_AUTOMOC ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)
find_package(protobuf REQUIRED CONFIG)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

qt_standard_project_setup()

add_subdirectory(include)
add_subdirectory(src)

option(BUILD_TESTS "" ON)
option(BUILD_EXAMPLE "" ON)
option(CREATE_DLL "" OFF)

if (BUILD_EXAMPLE)
    message(STATUS "Configuring example...")
    add_subdirectory(example)
endif()

if (BUILD_TESTS)
    message(STATUS "Configuring tests...")
    add_subdirectory(tests)
endif()