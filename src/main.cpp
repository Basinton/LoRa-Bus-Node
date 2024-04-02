#include <Arduino.h>

#include "button.h"
#include "eeprom.h"
#include "gps.h"
#include "led.h"
#include "lora.h"
#include "ota.h"
#include "ap.h"
#include "buzzer.h"

#include "bus.h"
uint32_t startCycle = 0;

void setup()
{
    delay(1000);
    Serial.begin(115200);

    button_init();
    led_init();

    gps_init();
    lora_init();
    
    bus_init();

    AP_init();

    buzzer_init();
    // buzzer_toggle_1s();
}

void loop()
{
    while (millis() - startCycle < 50)
        ;
    startCycle = millis();

    gps_process();
}
