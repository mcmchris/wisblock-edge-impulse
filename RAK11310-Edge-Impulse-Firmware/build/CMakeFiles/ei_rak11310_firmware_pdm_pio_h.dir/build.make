# CMAKE generated file: DO NOT EDIT!
# Generated by "MinGW Makefiles" Generator, CMake Version 3.24

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Disable VCS-based implicit rules.
% : %,v

# Disable VCS-based implicit rules.
% : RCS/%

# Disable VCS-based implicit rules.
% : RCS/%,v

# Disable VCS-based implicit rules.
% : SCCS/s.%

# Disable VCS-based implicit rules.
% : s.%

.SUFFIXES: .hpux_make_needs_suffix_list

# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

SHELL = cmd.exe

# The CMake executable.
CMAKE_COMMAND = "C:\Program Files\CMake\bin\cmake.exe"

# The command to remove a file.
RM = "C:\Program Files\CMake\bin\cmake.exe" -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = D:\MyRepositories\wisblock-edge-impulse\RAK11310-Edge-Impulse-Firmware

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = D:\MyRepositories\wisblock-edge-impulse\RAK11310-Edge-Impulse-Firmware\build

# Utility rule file for ei_rak11310_firmware_pdm_pio_h.

# Include any custom commands dependencies for this target.
include CMakeFiles/ei_rak11310_firmware_pdm_pio_h.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/ei_rak11310_firmware_pdm_pio_h.dir/progress.make

CMakeFiles/ei_rak11310_firmware_pdm_pio_h: pdm.pio.h

pdm.pio.h: D:/MyRepositories/wisblock-edge-impulse/RAK11310-Edge-Impulse-Firmware/ThirdParty/PDM/src/pdm.pio
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --blue --bold --progress-dir=D:\MyRepositories\wisblock-edge-impulse\RAK11310-Edge-Impulse-Firmware\build\CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Generating pdm.pio.h"
	pioasm\pioasm.exe -o c-sdk D:/MyRepositories/wisblock-edge-impulse/RAK11310-Edge-Impulse-Firmware/ThirdParty/PDM/src/pdm.pio D:/MyRepositories/wisblock-edge-impulse/RAK11310-Edge-Impulse-Firmware/build/pdm.pio.h

ei_rak11310_firmware_pdm_pio_h: CMakeFiles/ei_rak11310_firmware_pdm_pio_h
ei_rak11310_firmware_pdm_pio_h: pdm.pio.h
ei_rak11310_firmware_pdm_pio_h: CMakeFiles/ei_rak11310_firmware_pdm_pio_h.dir/build.make
.PHONY : ei_rak11310_firmware_pdm_pio_h

# Rule to build all files generated by this target.
CMakeFiles/ei_rak11310_firmware_pdm_pio_h.dir/build: ei_rak11310_firmware_pdm_pio_h
.PHONY : CMakeFiles/ei_rak11310_firmware_pdm_pio_h.dir/build

CMakeFiles/ei_rak11310_firmware_pdm_pio_h.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles\ei_rak11310_firmware_pdm_pio_h.dir\cmake_clean.cmake
.PHONY : CMakeFiles/ei_rak11310_firmware_pdm_pio_h.dir/clean

CMakeFiles/ei_rak11310_firmware_pdm_pio_h.dir/depend:
	$(CMAKE_COMMAND) -E cmake_depends "MinGW Makefiles" D:\MyRepositories\wisblock-edge-impulse\RAK11310-Edge-Impulse-Firmware D:\MyRepositories\wisblock-edge-impulse\RAK11310-Edge-Impulse-Firmware D:\MyRepositories\wisblock-edge-impulse\RAK11310-Edge-Impulse-Firmware\build D:\MyRepositories\wisblock-edge-impulse\RAK11310-Edge-Impulse-Firmware\build D:\MyRepositories\wisblock-edge-impulse\RAK11310-Edge-Impulse-Firmware\build\CMakeFiles\ei_rak11310_firmware_pdm_pio_h.dir\DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/ei_rak11310_firmware_pdm_pio_h.dir/depend

