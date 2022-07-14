# Edge Impulse firmware for RAK11200 (WisBlock Audio)


## Requirements

### Hardware

- [RAK1907](https://store.rakwireless.com/products/rak19007-wisblock-base-board-2nd-gen) or [RAK5005-O](https://store.rakwireless.com/products/rak5005-o-base-board) Base Board
- [RAK11200](https://store.rakwireless.com/products/wiscore-esp32-module-rak11200) ESP32 Core 
- [RAK18000](https://store.rakwireless.com/products/wisblock-microphone-module-rak18000) or RAK18030 PDM Microphone

### Tools

If you don't need to modify the firmware, just flash the already built firmware (ei) to start creating your voice/sounds recognition model

Install ESP IDF v4.4, following the instructions for your OS from [this page](https://docs.espressif.com/projects/esp-idf/en/v4.4/esp32/get-started/index.html#installation-step-by-step).

You'll need three additional components to compile this firmware:
- esp-camera
- LIS3DHTR_ESP-IDF
- ESP-NN

Get them by cloning the corresponding repositories to components folder in the root folder of the project:

```bash
cd components
git clone --recurse-submodules https://github.com/AIWintermuteAI/esp32-camera.git esp32-camera/
cd esp32-camera && git checkout 089d998a2c9a4fc21ca06513885e64cdefa4e177 && cd ..
git clone https://github.com/AIWintermuteAI/LIS3DHTR_ESP-IDF.git LIS3DHTR_ESP-IDF/
cd LIS3DHTR_ESP-IDF && git checkout 641bda8c3e4b706a2365fe87dd4d925f96ea3f8c && cd ..
git clone https://github.com/espressif/esp-nn.git esp-nn/
cd esp-nn && git checkout 24d18025f300c1e15afa2abb86519da54c7a5d90 && cd ..
cd ..
```


### Flash

Connect the ESP32 board to your computer.

Run:
   ```bash
   idf.py -p /dev/ttyUSB0 flash monitor
   ```

Where ```/dev/ttyUSB0``` needs to be changed to actual port where ESP32 is connected on your system.

### Serial connection



Additionally, since Edge Impulse firmware is open-source and available to public, if you have made modifications/added new sensors capabilities, we encourage you to make a PR in firmware repository!
