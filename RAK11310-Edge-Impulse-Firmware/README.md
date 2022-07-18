# Edge Impulse firmware for RAK11310 (RP2040) (WisBlock Audio)

## Requirements

### Hardware

- [RAK1907](https://store.rakwireless.com/products/rak19007-wisblock-base-board-2nd-gen) or [RAK5005-O](https://store.rakwireless.com/products/rak5005-o-base-board) Base Board
- [RAK11310](https://store.rakwireless.com/products/rak11310-wisblock-lpwan-module) RP2040 Core 
- [RAK18000](https://store.rakwireless.com/products/wisblock-microphone-module-rak18000) or RAK18030 PDM Microphone

### Step by Step guide (If you don't need to modify the firmware)

### Flash

- Download the already built [firmware .uf2](https://github.com/mcmchris/wisblock-edge-impulse/blob/main/RAK11310-Edge-Impulse-Firmware/build/ei_rak11310_firmware.uf2)
- Connect your WisBlock board to your PC using the USB cable.
- Hold the BootSel button in the RAK11300 module, reset the module (press the reset button of WisBlock Base Board for at least 2 seconds) then wait for the USB Mass Storage RPI-RP2 to appear before releasing the BootSel button.
<img src="https://user-images.githubusercontent.com/49886387/179538245-6d259c7e-b14f-4b06-b9c5-789edaed70c3.png" alt="drawing" width="400"/>

- Drag and drop the downloaded [ei_rak11310_firmware.uf2](https://github.com/mcmchris/wisblock-edge-impulse/blob/main/RAK11310-Edge-Impulse-Firmware/build/ei_rak11310_firmware.uf2) file to the RPI-RP2 storage and wait it reboots.

![image](https://user-images.githubusercontent.com/49886387/179539239-12b3a995-5f42-4d34-9ac6-ddfcd39129b1.png)

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
- For this you will need a serial terminal to send an AT command to the board (Tested with Putty and Tera term):
   ```bash
   AT+RUNIMPULSE
   ```
**Note:** this core doesn't support continuous mode yet, so you will need to say the phrase exactly just when you see the "Recording..." message in the terminal:

![image](https://user-images.githubusercontent.com/49886387/179541391-c61d848c-ce86-48ea-b123-4c671172153a.png)

- Another way is by installing the [Edge Impulse CLI](https://github.com/mcmchris/wisblock-edge-impulse/blob/main/RAK11200-Edge-Impulse-Firmware/README.md#install-edge-impulse-cli) <- here is the guide.
- Once you have the CLI already installed, type in the Command prompt:
   ```bash
   edge-impulse-run-impulse
   ```
- The model should run and start printing the inference results rapidly, it will seek for the phrase "Hey RAKstar", if heard, will print "Hey RAKstar heard" and will turn on the green LED in the WisBlock board.

![image](https://user-images.githubusercontent.com/49886387/179295206-2db2cdba-394c-4f56-9931-7a656d545bb1.png)


### Step by Step guide (If you need to customize this firmware or add new sensors support)

For **Windows** users follow this [guide](https://shawnhymel.com/2096/), then you will be able to open the project of this repository on Visual Studio and build it.
For **macOS** users follow this [guide](https://datasheets.raspberrypi.com/pico/getting-started-with-pico.pdf) (chapter 9.1 pag 36)

### Building the application

After installing the neccessary tools to build code for the RP2040.
Open the project on Visual Studio code.
Select the compiler that cames in the pico-sdk and click on "Build".

![Captura de pantalla 2022-07-18 114100](https://user-images.githubusercontent.com/49886387/179549644-c45c9b3b-9705-427e-8c72-4b2c44aef0e5.png)
If it fails, delete, delete manually the Build folder of the project and try building again.

### Flash

- Connect the RAK11310 RP2040 WisBlock board to your computer.
- Hold the BootSel button in the RAK11300 module, reset the module (press the reset button of WisBlock Base Board for at least 2 seconds) then wait for the USB Mass Storage RPI-RP2 to appear before releasing the BootSel button.
- Drag and drop the built fimrware .uf2 file to the RPI-RP2 storage and wait it reboots.
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

-Hit Enter and then define a custom name for your device (RAK11310 RP2040 in this case):

Additionally, since Edge Impulse firmware is open-source and available to public, if you have made modifications/added new sensors capabilities, we encourage you to make a PR in firmware repository!
