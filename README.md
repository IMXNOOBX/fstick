# 💫 FStick

![img](.github/showcase.jpeg)

This is a simple project to play around with the [M5StickC Plus](https://m5stack.com/products/m5stickc-plus-esp32-pico-mini-iot-development-kit) trying to mimic a flipper zero functionality. I bought it cause i couldn't afford the flipper zero myself :c. This project is based on many different projects credited below. Even though the project is published, it is still a work in progress, and I will continue working on it for a while. Feel free to contribute and share your incredible ideas.

## 💖 Donors
Thanks to all the donors for supporting this project! ❤
>🌟 [**Killer74-hub**](https://github.com/Killer74-hub) for a generous donation to keep this project going! Thank you so much! 

## 🪀 Usage
* The device has three different buttons, we will call them TOP (the one on the top), M5 (the one in the front) and BOTTOM (the one on the bottom).
* To Select an option, press the M5 button.
* To go to the last page/option, press the TOP button.
* To go to the next page/option, press the BOTTOM button.
* (TIP) If you mantain the M5 button pressed for 3 seconds, you will enter a secret menu where it will simulate a hadware wallet, and lock the device until restart.


## ➰ Features (WIP)
1. [x] IR Controller (TV, AC, etc)
	1. Spam ir codes to turn off tvs

2. [x] Wifi Utilities
	1. Scan Access Points
	2. Spam Access Points
	3. Clone Access Points
	4. Rogue Access Points
	5. Probe Request Spam
	6. Deauth Attack

3. [x] Bluetooth Utilities
	1. Apple BLE Spam
	2. Android BLE Spam (WIP)
	3. Windows BLE Spam
	4. Loop everyone BLE Spam

4. [x] Settings (WIP)
	1. Battery Saver
	2. Sound On/Off (WIP)
	3. Led On/Off
	4. Restart Device
	5. Shutdown Device

## 📦 How To Build
> [!IMPORTANT]
> You can build the project semi automatically following [`this guide`]()

### 🚀 Automatic Build
>[!WARNING]
> ***Windows/Linux only***

> Follow each step precisely to avoid any issues with the build process
1. Download [**Docker**](https://www.docker.com/products/docker-desktop/) and install it (*restart*)
2. Download [**Python**](https://www.python.org/downloads/) and install it
    1. Install dependencies using `pip install pyserial`
3. Download the **Respository** and move inside the folder 
4. Run the command `python3 setup/manager.py` to build the project
5. While the docker is building the project, you can connect the device to the computer (~5-10mins)
6. Once the docker finishes building, the project will be exported to the `build/` directory automatically
7. The script will ask you to select the port where the device is connected, select the correct one and press enter. if the device is not detected, you can use the flag `--flash` to skip the build and try again
8. The device will be flashed with the new firmware and you can start using it!
* If something goes wrong, i sugges you open an issue with detailed information about the error or try to build the project manually. this is just a helper script to make the process easier.



### 🧩 Manually Build
1. Clone the repo using `git clone https://github.com/IMXNOOBX/fstick.git`
2. Download the [**Arduino IDE**](https://www.arduino.cc/en/software) and install it or use visual studio code with the [**Arduino extension**](https://marketplace.visualstudio.com/items?itemName=vsciot-vscode.vscode-arduino)
3. Run the python script `python3 setup.py` to be able to to build the project
4. In your Arduino IDE or VSCode extension go to the main file `fstick.ino` and click on the build button in the top Left/Right in Arduino/VSCode respectively.

### 🐋 Build with Docker
*Asuming the repo is already downloaded and you have docker*
1. Run the command `docker build -t fstick-build .` to build the docker image
2. Run the following commands to export the project to your local machine
	```bash
	docker create --name temp-fstick-build fstick-build
	docker cp temp-fstick-build:/root/fstick/fstick.ino .
	docker rm temp-fstick-build
	```
3. The files will be exported to the `build/` directory


## 🧶 Contribute/Issues
Feel free to contribute or open any issue for the project, just make sure to read the [**CONTRIBUTE.md**](./CONTRIBUTE.md) file first and you follow the steps acordingly.

### Donations

Please consider donating, so i can keep improving this project and get a flipper zero to work on many more projects like this ❤

[![Donate](https://img.shields.io/badge/PayPal-00457C?style=for-the-badge&logo=paypal&logoColor=white)](https://paypal.me/itsxnoobx) [![Donate](https://img.shields.io/badge/Buy_Me_A_Coffee-FFDD00?style=for-the-badge&logo=buy-me-a-coffee&logoColor=black)](https://ko-fi.com/imxnoobx)

## 📚 Credits
* [nemo](https://github.com/n0xa/m5stick-nemo) project for the idea, assistance with the m5burner and useful code!
* [SourApple](https://github.com/RapierXbox/ESP32-Sour-Apple) for the ble spammer
* [wifi-penetration-tool](https://github.com/risinek/esp32-wifi-penetration-tool) for incredible well documented code and such great tools

# 🔖 License & Copyright

This project is licensed under [**CC BY-NC 4.0**](https://creativecommons.org/licenses/by-nc/4.0/).
```diff
+ You are free to:
	• Share: Copy and redistribute the material in any medium or format.
	• Adapt: Remix, transform, and build upon the material.
+ Under the following terms:
	• Attribution: You must give appropriate credit, provide a link to original the source repository, and indicate if changes were made.
	• Non-Commercial: You may not use the material for commercial purposes.
- You are not allowed to:
	• Sell: This license forbids selling original or modified material for commercial purposes.
	• Sublicense: This license forbids sublicensing original or modified material.
```
### ©️ Copyright
The content of this project is ©️ by [IMXNOOBX](https://github.com/IMXNOOBX) and the respective contributors. See the [LICENSE.md](LICENSE.md) file for details.
