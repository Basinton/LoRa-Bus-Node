# Driver Notify Device - LoRa Node

## Overview
This LoRa Node component is responsible for gathering data from the bus and transmitting it to the LoRa Gateway. It uses an ESP32 running on PlatformIO and communicates with sensors, collecting data like GPS coordinates, speed, and status.

## Setup
1. **PlatformIO Installation**  
   Install PlatformIO and the required libraries by following the official [PlatformIO documentation](https://platformio.org/install).

2. **Dependencies**  
   - Platform: espressif32
   - Board: esp32dev
   - Library:
     + LoRa_E32.h
     + WiFi.h
     + TinyGPSPlus.h
     + SoftwareSerial.h
     + AsyncTCP.h
     + ESPAsyncWebServer.h
     + Arduino_JSON.h
     + ArduinoWebsockets.h

   Ensure you have these dependencies in your `platformio.ini` file.

3. **Wiring**
   - Connect your sensors to the ESP32 as per the following:
     - Module GPS:
       + GPS_BAUDRATE    9600
       + GPS_RX          Pin 25
       + GPS_TX          Pin 23
     - Module Lora:
       + PIN_M0          Pin 18
       + PIN_M1          Pin 5
       + PIN_AUX         Pin 4

4. **Configuration**  
   Modify the `.h` file to reconfigure the relevant settings

## Usage
1. **Upload Firmware**  
   Upload the firmware using PlatformIO and ensure the node initializes successfully.

2. **Operation**  
   The node will continuously transmit bus information to the LoRa Gateway.

3. **Troubleshooting**
   - Check wiring if data is not collected.
   - Verify LoRa communication settings in `lora.h`.

