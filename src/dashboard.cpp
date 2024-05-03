// Import required libraries
#include "dashboard.h"

/* Variables --------------------------------------------------------------------*/
// SSID and password for AP Mode
const char *ap_ssid = "LORA_TESTING";
const char *ap_password = "12345678";

// SSID and password for STA Mode
const char *sta_ssid = DEFAULT_STA_SSID;
const char *sta_password = DEFAULT_STA_PASSWORD;

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);
// Create an Event Source on /events
AsyncWebSocket ws("/ws");

String message = "";

/* Task handles */
TaskHandle_t dashboardTaskHandle = NULL;

/* Functions -----------------------------------------------------------------*/
void dashboard_task(void *pvParameters)
{
  while (1)
  {
    ws.cleanupClients();
    vTaskDelay(pdMS_TO_TICKS(50));
  }
}

// Json Variable to Hold Bus Info
JSONVar busInfo;

// Get Bus Info
String getBusInfo()
{
  busInfo["bus_route"] = String(busRoute);
  busInfo["bus_driver_name"] = String(busDriverName);
  busInfo["bus_location_lat"] = String(gps_lat, 14);
  busInfo["bus_location_lng"] = String(gps_lng, 14);
  busInfo["bus_speed"] = String(gps_kmph);
  busInfo["bus_direction"] = String(busDirection);
  busInfo["bus_next_stop"] = String(nowBusStop);

  String jsonString = JSON.stringify(busInfo);
  return jsonString;
}

// Initialize SPIFFS
void fs_init()
{
  if (!SPIFFS.begin())
  {
    Serial.println("An error has occurred while mounting SPIFFS");
  }
  else
  {
    sprintf(serial_buffer, "%-10s %-15s", "SPIFFS:", "Initialized");
    Serial.println(serial_buffer);
  }
}

// Initialize WiFi
void wifi_init()
{
  WiFi.mode(WIFI_STA);
  WiFi.disconnect(true);
  // // Start the AP
  // WiFi.softAP(ap_ssid, ap_password);
  // Serial.print("Access Point \"");
  // Serial.print(ap_ssid);
  // Serial.println("\" started");
  // IPAddress IP = WiFi.softAPIP();
  // Serial.println(IP);

  // Connect to the STA
  WiFi.begin(sta_ssid, sta_password);
  Serial.print("Connecting WiFi..");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void notifyClients(String busInfo)
{
  ws.textAll(busInfo);
}

void handleWebSocketMessage(void *arg, uint8_t *data, size_t len)
{
  AwsFrameInfo *info = (AwsFrameInfo *)arg;
  if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT)
  {
    data[len] = 0;
    message = (char *)data;
    // Serial.println(message);
    if (strcmp((char *)data, "getBusInfo") == 0)
    {
      notifyClients(getBusInfo());
    }
  }
}

void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len)
{
  switch (type)
  {
  case WS_EVT_CONNECT:
    Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
    break;
  case WS_EVT_DISCONNECT:
    Serial.printf("WebSocket client #%u disconnected\n", client->id());
    break;
  case WS_EVT_DATA:
    handleWebSocketMessage(arg, data, len);
    break;
  case WS_EVT_PONG:
  case WS_EVT_ERROR:
    break;
  }
}

void websocket_init()
{
  ws.onEvent(onEvent);
  server.addHandler(&ws);
}

void dashboard_init(void)
{
  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(SPIFFS, "/index.html", "text/html"); });

  server.serveStatic("/", SPIFFS, "/");

  server.begin();

  // Create the dashboard task
  xTaskCreate(dashboard_task, "Dashboard Task", 1024, NULL, 2, &dashboardTaskHandle);

  sprintf(serial_buffer, "%-10s %-15s", "DASHBOARD:", "Initialized");
  Serial.println(serial_buffer);
}