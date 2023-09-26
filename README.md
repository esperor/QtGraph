# QtGraph
Qt6 based widget. Inspired by Unreal Engine blueprints, this widget is capable of constructing complex graphs. There is plan to make visual programming -like logic gates and ability to create and even execute algorithms (see https://github.com/users/esperor/projects/2).

# Dependencies

1. Qt6: Core, Gui, Widgets
2. VulkanSDK (somehow)
3. googletest (for tests)
4. Python 3 (for the googletest)
5. protobuf (tested up to 21.12 because of troubles with abseil after 22.0)

# Building

1. Open root CMakeLists.txt and set Vulkan_INCLUDE_DIR for your VulkanSDK installation
2. Build and install preffered version of protobuf (generally `cmake --build . --target install`): https://github.com/protocolbuffers/protobuf/blob/main/cmake/README.md
3. Open terminal in root directory, create build directory, cd inside it
4. From build folder run ``` cmake .. ``` (add `-DBUILD_EXAMPLE=OFF` and `-DBUILD_TESTS=OFF` if needed)
5. Run ``` cmake --build . ```
6. For tests run ``` ctest --test-dir tests ```

# Notes about building

If not installed you'll need to install protobuf (``` cmake --build . --target install ```). Also it might be neccesary to add `protobuf/bin` (protoc.exe here) folder to your PATH variable. 
As noted in the Dependencies, tested protobuf version is up to 21.12 - after this version, protobuf adds abseil as its dependency and it's too complicated so far.

Tested with Visual Studio generator, Qt6.6.0 x64.
