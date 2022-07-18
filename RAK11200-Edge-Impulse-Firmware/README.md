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
- Open the Flash Download Tool and set chipType to "ESP32" and left workMode on "develop", finally click in OK.

![image](https://user-images.githubusercontent.com/49886387/179289799-e3328d8b-f8e2-4e99-a4af-6028eb6cdffa.png)

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

- Click on START and wait until you see the Finished status in the Download tool.

![image|690x285](https://user-images.githubusercontent.com/49886387/179290381-4ba5a259-6442-4b73-bac0-dbacd45f76e0.png)

![image|690x285](https://user-images.githubusercontent.com/49886387/179116885-5ac376b1-b176-4ce3-95f7-aeeca5cfc5b4.png)

### Test

To upload data to Edge Impulse for the first time, you will need to:
1. Create an [Edge Impulse account](https://studio.edgeimpulse.com/login).
2. Create your first project:

![Captura de pantalla 2022-07-15 160739](https://user-images.githubusercontent.com/49886387/179303223-b7f7e2f6-53df-4195-8d09-87d3ae7d1283.png)

3. Go to Data acquisition tab in the left menu:

![Captura de pantalla 2022-07-14 193434](https://user-images.githubusercontent.com/49886387/179118482-f69b6150-ece9-4be0-9fe1-3ec638e204e8.png)

4. In the Record new data section, click on "Connect using WebUSB" just work for "Google Chrome or Microsoft Edge" ([not a Google Chrome or Microsoft Edge user?](https://github.com/mcmchris/wisblock-edge-impulse/blob/main/RAK11200-Edge-Impulse-Firmware/README.md#install-edge-impulse-cli)):

![Captura de pantalla 2022-07-14 193612](https://user-images.githubusercontent.com/49886387/179118604-4f8a15cf-893e-4207-a0c5-306bae7ce9ae.png)

5. In the Pop-up window, select the device and click in connect.

![Captura de pantalla 2022-07-14 193738](https://user-images.githubusercontent.com/49886387/179118781-b0cef568-7815-4ed5-a23e-2979b1eb9f1a.png)

6. Finally, after defining the sensor, name the sample with a label and define the sample length, click on "Start Sampling".

![Captura de pantalla 2022-07-14 194308](https://user-images.githubusercontent.com/49886387/179119291-15b8d0aa-2dcd-47f2-858c-ebfc6c81d44a.png)

7. Say something you want to record and there you will have your first playable audio sample:

![image](https://user-images.githubusercontent.com/49886387/179119451-32392dd1-6ebe-404f-a749-ee39748fa6dd.png)

### Voice recognition model test
The firmware has an already trained model inside ready to be tested.
- For this you will need a serial terminal to send an AT command to the board (Arduino IDE Serial Terminal for this example with NL & CR line ending):
   ```bash
   AT+RUNIMPULSECONT
   ```
   ![image](https://user-images.githubusercontent.com/49886387/179533317-90f8f0d3-9d26-499e-aa97-8af6e02830b2.png)

- Another way is by installing the [Edge Impulse CLI](https://github.com/mcmchris/wisblock-edge-impulse/blob/main/RAK11200-Edge-Impulse-Firmware/README.md#install-edge-impulse-cli) <- here is the guide.
- Once you have the CLI already installed, type in the Command prompt:
   ```bash
   edge-impulse-run-impulse --continuous
   ```
- The model should run and start printing the inference results rapidly, it will seek for the phrase "Hey RAKstar", if heard, will print "Hey RAKstar heard" and will turn on the green LED in the WisBlock board.

![image](https://user-images.githubusercontent.com/49886387/179295206-2db2cdba-394c-4f56-9931-7a656d545bb1.png)


### Step by Step guide (If you need to customize this firmware or add new sensors support)

Install ESP IDF v4.4, following the instructions for your OS from [this page](https://docs.espressif.com/projects/esp-idf/en/v4.4/esp32/get-started/index.html#installation-step-by-step).

- Download this project or clone it.
- You'll need two additional components to compile this firmware:
  - LIS3DHTR_ESP-IDF
  - ESP-NN
-First open the ESP-IDF 4.4 CMD from the Desktop shortcut.
-Then Navigate to the firmware folder ../RAK11200-Edge-Impulse-Firmware

Get the additional components by cloning the corresponding repositories to **components** folder in the root folder of the project: (../RAK11200-Edge-Impulse-Firmware/components)

```bash
cd components
git clone https://github.com/AIWintermuteAI/LIS3DHTR_ESP-IDF.git LIS3DHTR_ESP-IDF/
cd LIS3DHTR_ESP-IDF 
git checkout 641bda8c3e4b706a2365fe87dd4d925f96ea3f8c 
cd ..
git clone https://github.com/espressif/esp-nn.git esp-nn/
cd esp-nn 
git checkout 24d18025f300c1e15afa2abb86519da54c7a5d90 
cd ..
cd ..
```
With this last "cd .." you must be in the project folder ../RAK11200-Edge-Impulse-Firmware

### Building the application

Run:
   ```bash
   idf.py build
   ```

### Flash

- Connect the RAK11200 ESP32 WisBlock board to your computer.
- Short BOOT0 and GND then press Reset button of the base board. (To re-start the core in boot mode)
- Identify the COM port where it's connected.
Run:
   ```bash
   idf.py -p COMxx flash monitor
   ```
- Where ```COMxx``` needs to be changed to actual port where your WisBlock is connected on your system.
- After uploaded successfully, reset the board.
- Finally, start sending voice samples through the [CLI](https://github.com/mcmchris/wisblock-edge-impulse/blob/main/RAK11200-Edge-Impulse-Firmware/README.md#install-edge-impulse-cli) or directly with the [WebUSB](https://github.com/mcmchris/wisblock-edge-impulse/blob/main/RAK11200-Edge-Impulse-Firmware/README.md#test) function from your browser, or test your trained model by [Running the Impulse](https://github.com/mcmchris/wisblock-edge-impulse/blob/main/RAK11200-Edge-Impulse-Firmware/README.md#voice-recognition-model-test)

### Install Edge Impulse CLI
- Follow the [installation guide](https://docs.edgeimpulse.com/docs/edge-impulse-cli/cli-installation#installation-macos-and-windows) for your OS.
- Once installed, open your favorite command prompt and type "edge-impulse-daemon" with your board connected to your PC.
   ```bash
   edge-impulse-daemon
   ```
- Enter your Edge Impulse account email or username and your password.

![image](https://user-images.githubusercontent.com/49886387/179292586-e30bb973-e332-48b9-a37a-00fd5e95ab23.png)

-Hit Enter and then you will be asked to select a project to bind your device (WisBlock Audio in this case):

![image](https://user-images.githubusercontent.com/49886387/179292797-08c7d290-c8f7-408b-a745-2c93f8874acc.png)

-Hit Enter and then define a custom name for your device (RAK11200 ESP32 in this case):

![Captura de pantalla 2022-07-15 145926](https://user-images.githubusercontent.com/49886387/179293189-da624b61-3eb3-485b-bfa2-0808c2ca295b.png)



Additionally, since Edge Impulse firmware is open-source and available to public, if you have made modifications/added new sensors capabilities, we encourage you to make a PR in firmware repository!
