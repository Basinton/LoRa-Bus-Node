#ifndef __DASHBOARD_H_
#define __DASHBOARD_H_

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "bus.h"
/* Define --------------------------------------------------------------------*/
// SSID and password for AP Mode
#define DEFAULT_AP_SSID "LORA_TESTING"
#define DEFAULT_AP_PASSWORD "12345678"

// SSID and password for STA Mode
#define DEFAULT_STA_SSID "BKIT_LUGIA_CS2"
#define DEFAULT_STA_PASSWORD "cselabc5c6"



void wifi_init(void);
void fs_init(void);
void websocket_init(void);
void dashboard_init(void);
String getBusInfo();
void reconnectWebServer();

#endif // DASHBOARD_H
