#ifndef __GPS_H_
#define __GPS_H_

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Define --------------------------------------------------------------------*/
#define GPS_BAUDRATE 9600
#define GPS_RX       25
#define GPS_TX       23

/* Variables -----------------------------------------------------------------*/
extern TinyGPSPlus gps;

extern float gps_lat;
extern float gps_lng;
extern float gps_meters;
extern float gps_kmph;

/* Functions -----------------------------------------------------------------*/
void gps_init();

#endif