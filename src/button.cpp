#include "button.h"

/* Define --------------------------------------------------------------------*/

/* Variables -----------------------------------------------------------------*/
uint32_t keyCode = 0;
uint32_t keyTimeBeforeReleasing = 0;

/* Task handles */
TaskHandle_t buttonTaskHandle = NULL;

/* Functions -----------------------------------------------------------------*/
void button_task(void *pvParameters)
{
    unsigned long previousMillis = 0;

    while (1)
    {
        if (digitalRead(BUTTON_PIN) == LOW)
        {
            keyTimeBeforeReleasing = 0;
            keyCode++;
        }
        else
        {
            keyTimeBeforeReleasing = keyCode;
            keyCode = 0;
        }

        vTaskDelay(pdMS_TO_TICKS(50));
    }
}

void button_init()
{
    pinMode(BUTTON_PIN, INPUT_PULLUP);
    keyCode = 0;
    keyTimeBeforeReleasing = 0;

    // Create the button task
    xTaskCreate(button_task, "Button Task", 2048, NULL, 2, &buttonTaskHandle);

    sprintf(serial_buffer, "%-10s %-15s", "BUTTON:", "Initialized");
    Serial.println(serial_buffer);
}

uint32_t button_keycode()
{
    return keyCode;
}

uint32_t button_keycode_release()
{
    return keyTimeBeforeReleasing;
}