#include "ap.h"
#include "html/html.h"
#include "gps.h"
#include "bus.h"
#include <Arduino.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

AsyncWebServer my_server(80);

char ap_ssid[32]    = AP_SSID_DEFAULT;
char ap_password[9] = AP_PASSWORD_DEFAULT;
char ap_ssid_temp[32];

bool loggedIn;
unsigned long loginTimeout;
unsigned long apTimeout;

char latStr[20];
char lngStr[20];
char kmphStr[20];

void AP_init()
{
    // Set timeout
    apTimeout = millis() + AP_TIMEOUT;
    loggedIn  = false;

    // Disconnect from STA mode
    WiFi.disconnect(true); // true parameter to also disconnect from the AP mode

    delay(100);

    // Set AP SSID by using ID of Device
    // ...

    // Connect AP mode
    WiFi.mode(WIFI_AP);
    WiFi.softAP(ap_ssid, ap_password);

    IPAddress IP = WiFi.softAPIP();

    Serial.print("ap: \t [init] ");
    Serial.println(IP);

    // Handle requests for /favicon.ico
    my_server.on("/favicon.ico", HTTP_GET, [](AsyncWebServerRequest *request)
                 {
                     request->send(404); // Return a 404 status code
                 });

    // Setting Page
    my_server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
                 {
        request->send_P(200, "text/html", html_setting); 
        apTimeout = millis() + AP_TIMEOUT; });

    my_server.on("/", HTTP_POST, [](AsyncWebServerRequest *request)
                 {
        request->send_P(200, "text/html", html_setting);
        apTimeout = millis() + AP_TIMEOUT; });

    // GET DATA
    my_server.on("/data", HTTP_GET, [](AsyncWebServerRequest *request) {
        // Convert float variables to strings with 6 decimal places
        char latStr[15]; // Sufficient to hold a float with 6 decimal places
        char lngStr[15];
        char kmphStr[15];

        sprintf(latStr, "%.6f", gps_lat);
        sprintf(lngStr, "%.6f", gps_lng);
        sprintf(kmphStr, "%.2f", gps_kmph);

        // Create a String by concatenating the formatted strings with additional data
        String data = String(latStr) + "," + String(lngStr) + "," + String(kmphStr) + "," + String(busDirection) + "," + String(nowBusStop) + "," + String(preBusStop);

        
        request->send(200, "text/plain", data);
        
        apTimeout = millis() + AP_TIMEOUT;
    });


    my_server.begin();
}

void AP_process()
{
    if (loggedIn && millis() > loginTimeout)
    {
        loggedIn = false;
    }

    // if (millis() > apTimeout) {
    //     Serial.println("ap: \t [timeout] Back to Normal mode");
    // }
}