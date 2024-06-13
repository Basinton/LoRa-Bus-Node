#include "ap.h"
#include <WebServer.h>

TaskHandle_t APTaskHandle = NULL;

WebServer server(80);

// SSID and password for AP Mode
const char *ap_ssid = "LORA_BUS50";
const char *ap_password = "12345678";

// Initialize WiFi
void wifi_init()
{
  WiFi.mode(WIFI_AP);
  WiFi.disconnect(true);

  // Start the AP
  WiFi.softAP(ap_ssid, ap_password);
  IPAddress IP = WiFi.softAPIP();
  Serial.println(IP);
}

const char *htmlForm = R"rawliteral(
<!DOCTYPE HTML>
<html>
  <head>
    <title>ESP32 Station Configuration</title>
    <meta name="viewport" content="width=device-width, initial-scale=1">
  </head>
  <body>
    <h1>ESP32 Station Configuration</h1>
    <form action="/connect" method="POST">
      <label for="stationLat">Station Latitude:</label>
      <input type="text" id="stationLat" name="stationLat"><br><br>
      <label for="stationLng">Station Longitude:</label>
      <input type="text" id="stationLng" name="stationLng"><br><br>
      <input type="submit" value="Update">
    </form>
    <form action="/reboot" method="POST" style="margin-top:20px;">
      <input type="submit" value="Reboot ESP32">
    </form>
  </body>
</html>
)rawliteral";

void handleRoot()
{
    server.send(200, "text/html", htmlForm);
}

void handleConnect()
{
    String stationLatStr = server.arg("stationLat");
    String stationLngStr = server.arg("stationLng");

    // Update station latitude and longitude
    stationLat = stationLatStr.toFloat();
    stationLng = stationLngStr.toFloat();

    Serial.print("Station Latitude: ");
    Serial.println(stationLat);
    Serial.print("Station Longitude: ");
    Serial.println(stationLng);

    // Send a confirmation response to the client
    server.send(200, "text/html", "<html><body><h1>Parameters updated!</h1></body></html>");
}

void handleReboot()
{
    server.send(200, "text/html", "<html><body><h1>Rebooting ESP32...</h1></body></html>");
    delay(1000); // Give the web server time to send the response
    ESP.restart(); // Reboot the ESP32
}

void AP_task(void *pvParameters)
{
    while (1)
    {
        server.handleClient();
        vTaskDelay(pdMS_TO_TICKS(50));
    }
}

void AP_init()
{
    // Set up the web server
    server.on("/", handleRoot);
    server.on("/connect", HTTP_POST, handleConnect);
    server.on("/reboot", HTTP_POST, handleReboot);
    server.begin();
    Serial.println("HTTP server started");

    xTaskCreate(AP_task, "AP Task", 4096, NULL, configMAX_PRIORITIES - 1, &APTaskHandle);
}
