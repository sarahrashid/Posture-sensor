# Introduction 
This is the code for my Azure Internet of Things enabled wearable posture sensor device. The device sends data to Azure IoT Central or Hub about a user's posture by measuring the level of strain on the strain gauge worn on the user's back. Once the user surpasses the set threshold, the device will vibrate alerting the user to stand up straight. The threshold number at which the buzzer vibrates and the operating state (on/off) are configurable through IoT Central or Hub.

To create and run this project I used: C, Make, Ubuntu, FreeRTOS, ESP8266 wifi chip, and a AdaFruit Feather Huzzah board.

# Getting Started
TODO: Guide users through getting your code up and running on their own system. In this section you can talk about:
1. Install WSL for Ubuntu
2. Follow the instructions for Linux to install ESP IDF toolchain for ESP8266 and dev environment: https://docs.espressif.com/projects/esp8266-rtos-sdk/en/latest/get-started/index.html 
3. Update build scripts to point to the correct folders
4. Run Make and kconfig

A picture of the inside of the box with electrical components (worn with belt clip):
![Image of Box](https://imgur.com/hXOiTut)

How it looks to configure threshold and operating state in IoT Central: 
![Image of IoT Central](https://imgur.com/OkPHtp4)
