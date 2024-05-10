/* Includes ------------------------------------------------------------------*/
#include "gps.h"

/* Define --------------------------------------------------------------------*/

/* Variables -----------------------------------------------------------------*/
TinyGPSPlus gps;
SoftwareSerial gps_ss(GPS_RX, GPS_TX);

TaskHandle_t gpsTaskHandle = NULL;

float gps_lat = 0.0;
float gps_lng = 0.0;
float gps_meters = 0.0;
float gps_kmph = 0.0;

/* Functions -----------------------------------------------------------------*/
void gps_task(void *pvParameters)
{
    (void)pvParameters; // Avoid compiler warnings about the unused parameter

    while (1)
    {
        while (gps_ss.available())
        {
            gps.encode(gps_ss.read());
            vTaskDelay(pdMS_TO_TICKS(1));
        }

        // Update GPS variables
        myBus.busLat = gps.location.lat();
        myBus.busLong = gps.location.lng();
        // gps_meters = gps.altitude.meters();
        myBus.busSpeed = gps.speed.kmph();

        // Optional: Print GPS data for debugging
        // Serial.printf("gps: \t [get] %.6f, %.6f, %.2f m, %.2f km/h\n",
        //               gps_lat,
        //               gps_lng,
        //               gps_meters,
        //               gps_kmph);

        vTaskDelay(pdMS_TO_TICKS(1000)); // Adjust the delay duration as needed
    }
}

void gps_init()
{
    gps_ss.begin(GPS_BAUDRATE);
    xTaskCreate(gps_task, "GPS Task", 2048, NULL, 1, &gpsTaskHandle);

    sprintf(serial_buffer, "%-10s %-15s", "GPS:", "Initialized");
    Serial.println(serial_buffer);
}