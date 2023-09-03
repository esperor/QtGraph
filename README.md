# QtGraph
Qt6 based widget. Can construct a complex node-based graph (currently not very client friendly, but example can show most of the features).

# Dependencies

1. Qt6: Core, Gui, Widgets
2. VulkanSDK (somehow)
3. googletest (for tests)
4. Python 3 (for the googletest)
5. protobuf (tested up to 21.12 because of troubles with abseil after 22.0)

# Building

1. Open root CMakeLists.txt and set Vulkan_INCLUDE_DIR for your VulkanSDK installation
2. Open terminal in root directory, create build directory, cd inside it
3. From build folder run ``` cmake .. ``` (add `-DBUILD_EXAMPLE=OFF` and `-DBUILD_TESTS=OFF` if needed)
4. Run ``` cmake --build . ```
5. For tests run ``` ctest --test-dir tests ```

# Notes about building

If not installed you'll need to install protobuf (``` cmake --build . --target install ```). Also it might be neccesary to add `protobuf/bin` (protoc.exe here) folder to your PATH variable. 
As noted in the Dependencies, tested protobuf version is up to 21.12 - after this version, protobuf adds abseil as its dependency and i just couldn't resolve it. Any help with that would be appreciated.

Tested with MinGW Makefiles and Visual Studio generators, Qt6.5.2 x64 (executed from x64 Native Tools Command Prompt for VS 2022).

# Run example

Needed Qt dll's will already be copied into exe's directory. You need only to copy `platforms` directory from `Qt/<version>/<configuration>/plugins`.
