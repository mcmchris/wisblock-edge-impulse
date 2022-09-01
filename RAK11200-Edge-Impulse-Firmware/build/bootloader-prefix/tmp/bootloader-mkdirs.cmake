# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "C:/Users/MCMCHRIS/esp/esp-idf/components/bootloader/subproject"
  "C:/wisblock-edge-impulse/RAK11200-Edge-Impulse-Firmware/build/bootloader"
  "C:/wisblock-edge-impulse/RAK11200-Edge-Impulse-Firmware/build/bootloader-prefix"
  "C:/wisblock-edge-impulse/RAK11200-Edge-Impulse-Firmware/build/bootloader-prefix/tmp"
  "C:/wisblock-edge-impulse/RAK11200-Edge-Impulse-Firmware/build/bootloader-prefix/src/bootloader-stamp"
  "C:/wisblock-edge-impulse/RAK11200-Edge-Impulse-Firmware/build/bootloader-prefix/src"
  "C:/wisblock-edge-impulse/RAK11200-Edge-Impulse-Firmware/build/bootloader-prefix/src/bootloader-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "C:/wisblock-edge-impulse/RAK11200-Edge-Impulse-Firmware/build/bootloader-prefix/src/bootloader-stamp/${subDir}")
endforeach()
