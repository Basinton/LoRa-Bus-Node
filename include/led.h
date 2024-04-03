#ifndef __LED_H_
#define __LED_H_

/* Includes ------------------------------------------------------------------*/

/* Define --------------------------------------------------------------------*/
#define PIN_ALARM   22 // use this 21 old

/* Variables -----------------------------------------------------------------*/

/* Functions -----------------------------------------------------------------*/
void led_init();

void led_off();
void led_on();
void led_toggle_1s();

#endif