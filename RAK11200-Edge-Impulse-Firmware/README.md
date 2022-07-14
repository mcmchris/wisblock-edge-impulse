# Edge Impulse firmware for RAK11200 (WisBlock Audio)


## Requirements

### Hardware

- [RAK1907](https://store.rakwireless.com/products/rak19007-wisblock-base-board-2nd-gen) or [RAK5005-O](https://store.rakwireless.com/products/rak5005-o-base-board) Base Board
- [RAK11200](https://store.rakwireless.com/products/wiscore-esp32-module-rak11200) ESP32 Core 
- [RAK18000](https://store.rakwireless.com/products/wisblock-microphone-module-rak18000) or RAK18030 PDM Microphone

### Step by Step guide (If you don't need to modify the firmware)

### Flash

- Download the already built [firmware .bin](https://github.com/mcmchris/wisblock-edge-impulse/blob/main/RAK11200-Edge-Impulse-Firmware/build/ei_rak11200_firmware.bin)
- Download the [bootloader .bin](https://github.com/mcmchris/wisblock-edge-impulse/blob/main/RAK11200-Edge-Impulse-Firmware/build/bootloader/bootloader.bin) 
- Download the [partition table .bin](https://github.com/mcmchris/wisblock-edge-impulse/blob/main/RAK11200-Edge-Impulse-Firmware/build/partition_table/partition-table.bin)
- Download the [ESP Flash Download Tool](https://www.espressif.com/en/support/download/other-tools) or any other flasher compatible with your OS.
- Search for the ei_rak11200_firmware.bin and set it on @ 0x10000 partition
- Search for the bootloader.bin and set it on @ 0x1000 partition
- Search for the partition-table.bin and set it on @ 0x8000 partition
- Set the SPI SPEED to 40MHz, SPI MODE to DIO and DoNotChgBin checked.
Everything should look like this:

![image|690x285](https://user-images.githubusercontent.com/49886387/179113960-66323497-6fc8-49f5-a953-dd94b39b1a36.png)

- Connect your WisBlock board to your PC using the USB cable.
- Select the COM port where your board is connected and it's baud rate.

![image](https://user-images.githubusercontent.com/49886387/179116490-f4fc1ef7-0481-406d-8208-da069d252075.png)

- Short the BOOT0 and GND pins then reset the board to re-start the core in boot mode.

![image|690x285](https://user-images.githubusercontent.com/49886387/179116270-87070584-3172-4faa-9326-27bb337afb61.png)

- Click on Flash and wait until you see the Finished status in the Download tool.

![image|690x285](https://user-images.githubusercontent.com/49886387/179116885-5ac376b1-b176-4ce3-95f7-aeeca5cfc5b4.png)

### Test

To upload data to Edge Impulse for the first time, you will need to:
1. Create an [Edge Impulse account](https://studio.edgeimpulse.com/login).
2. Create your first project:

![image](https://user-images.githubusercontent.com/49886387/179117484-70429e61-dc6f-4563-b5fb-8a1170dcac16.png)

3. Go to Data acquisition tab in the left menu:

![Captura de pantalla 2022-07-14 193434](https://user-images.githubusercontent.com/49886387/179118482-f69b6150-ece9-4be0-9fe1-3ec638e204e8.png)

4. In the Record new data section, click on "Connect using WebUSB" just work for "Google Chrome or Microsoft Edge":

![Captura de pantalla 2022-07-14 193612](https://user-images.githubusercontent.com/49886387/179118604-4f8a15cf-893e-4207-a0c5-306bae7ce9ae.png)

5. In the Pop-up window, select the device and click in connect.

![Captura de pantalla 2022-07-14 193738](https://user-images.githubusercontent.com/49886387/179118781-b0cef568-7815-4ed5-a23e-2979b1eb9f1a.png)

6. Finally, after defining the sensor, name the sample with a label and define the sample length, click on "Start Sampling".

![Captura de pantalla 2022-07-14 194308](https://user-images.githubusercontent.com/49886387/179119291-15b8d0aa-2dcd-47f2-858c-ebfc6c81d44a.png)

7. Say something you want to record and there you will have your first playable audio sample:

![image](https://user-images.githubusercontent.com/49886387/179119451-32392dd1-6ebe-404f-a749-ee39748fa6dd.png)




#### If you need to customize this firmware or add new sensors support, these are the steps for you:

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
