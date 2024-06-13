#ifndef __AP_H_
#define __AP_H_

/* Includes ------------------------------------------------------------------*/
#include "global.h"
/* Define --------------------------------------------------------------------*/
// SSID and password for AP Mode
#define DEFAULT_AP_SSID "LORA_BUS_50"
#define DEFAULT_AP_PASSWORD "12345678"

// SSID and password for STA Mode
#define DEFAULT_STA_SSID "BKIT_LUGIA_CS2"
#define DEFAULT_STA_PASSWORD "cselabc5c6"

void wifi_init();
void AP_init();

#endif // DASHBOARD_H
