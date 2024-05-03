#ifndef __MAIN_H_
#define __MAIN_H_

/* Includes ------------------------------------------------------------------*/
#include <Arduino.h>
#include <stdint.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <LoRa_E32.h>
#include <WiFi.h>
#include <TinyGPSPlus.h>
#include <SoftwareSerial.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <Arduino_JSON.h>
#include <SPIFFS.h>

// Local Libs
#include "bus.h"
#include "button.h"
#include "buzzer.h"
#include "eeprom.h"
#include "gps.h"
#include "led.h"
#include "lora.h"
#include "ota.h"
#include "dashboard.h"

extern char serial_buffer[50]; // Adjust the size as necessary

#endif