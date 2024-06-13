#ifndef __BUZZER_H_
#define __BUZZER_H_

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Define --------------------------------------------------------------------*/
typedef struct {
	int frequency;
	int  duration;
} My_ToneTypeDef;

/* Variables -----------------------------------------------------------------*/
extern My_ToneTypeDef tones_5beep[];
extern My_ToneTypeDef tones_SMB[];
extern My_ToneTypeDef tones_startup[];
extern My_ToneTypeDef tones_turnoff[];
extern My_ToneTypeDef tones_3beep[];
extern My_ToneTypeDef tones_2_beep[];
extern My_ToneTypeDef single_beep[];

extern uint8_t buzzer_count;

/* Functions -----------------------------------------------------------------*/
void buzzer_init();
void buzzer_play_a_tone(My_ToneTypeDef* input_tone);

void buzzer_set_play_a_tone();
void buzzer_toggle_1s();
void buzzer_off();

#endif