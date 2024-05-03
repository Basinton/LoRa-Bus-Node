/* Includes ------------------------------------------------------------------*/
#include "main.h"

uint32_t startCycle = 0;
char serial_buffer[50];

void setup()
{
    Serial.begin(115200);
    wifi_init();
    button_init();
    led_init();
    gps_init();
    lora_init();
    buzzer_init();
    bus_init();
    fs_init();
    websocket_init();
    dashboard_init();
    buzzer_play_a_tone(tones_startup);
}

void loop()
{

}
