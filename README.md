# QtGraph
Qt6 based widget. Can construct a complex node-based graph (currently not very client friendly, but example can show most of the features).

# Dependencies

Qt6: Core, Gui, Widgets
VulkanSDK (somehow)
googletest (for tests)
Python 3 (for the googletest)

# Building:

1. Open root CMakeLists.txt and set Vulkan_INCLUDE_DIR for your VulkanSDK installation
2. Open terminal in root folder, create build folder, cd inside it
3. From build folder run ``` cmake .. ``` then ``` cmake --build . ```

Tested with MinGW Makefiles generator, Qt6.5.2 x64 (executed from x64 Native Tools Command Prompt for VS 2022).
Also tested with Visual Studio 2022, but for the tests to work you'll need to copy .dll manually into build/tests as I couldn't find proper way to do it for both debug and release configurations (help will be highly appreciated)

# Run example

After building you'll need to copy some dll's in folder with QtGraphExample.exe. So open build/example and copy in there ``` platforms ```folder from ``` Qt/version/<kit>/plugins/ ``` and ```QtCore.dll```, ```QtGui.dll```, ```QtWidgets.dll``` from ``` Qt/version/<kit>/bin ```.
QtGraph.dll will be already copied for most generators but VS 2022.
