#include "led.h"
#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

/* Define --------------------------------------------------------------------*/
typedef enum
{
    LED_ON,
    LED_OFF,
    LED_TOGGLE_1S
} LED_STATE;

/* Variables -----------------------------------------------------------------*/
LED_STATE led_state = LED_OFF;

unsigned long led_currentMillis  = 0;
unsigned long led_previousMillis = 0;

/* Task handles */  
TaskHandle_t ledTaskHandle = NULL;

/* Functions -----------------------------------------------------------------*/
void led_on()
{
    led_state = LED_ON;
}

void led_off()
{
    led_state = LED_OFF;
}

void led_toggle_1s()
{
    led_state = LED_TOGGLE_1S;
}

void led_task(void *pvParameters)
{
    while (1)
    {
        switch (led_state)
        {
            case LED_ON:
                digitalWrite(PIN_ALARM, LOW);

                // digitalWrite(32, HIGH);
                break;

            case LED_OFF:
                digitalWrite(PIN_ALARM, HIGH);

                // digitalWrite(32, LOW);
                break;

            case LED_TOGGLE_1S:
                led_currentMillis = millis();
                if (led_currentMillis - led_previousMillis >= 1000)
                {
                    led_previousMillis = led_currentMillis;
                    digitalWrite(PIN_ALARM, !digitalRead(PIN_ALARM));

                    // digitalWrite(32, !digitalRead(32));
                }
                break;

            default:
                led_state = LED_OFF;
                break;
        }

        vTaskDelay(pdMS_TO_TICKS(50));
    }
}

void led_init()
{
    pinMode(PIN_ALARM, OUTPUT);
    led_off();

    // pinMode(35, INPUT);
    // pinMode(32, OUTPUT);
    // digitalWrite(32, LOW);

    xTaskCreate(led_task, "LED Task", 4096, NULL, 2, &ledTaskHandle);

    Serial.println("led: \t [init]");
}
