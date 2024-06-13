#include "buzzer.h"

#define BUZZER_PIN 13
#define BUZZER_CHANNEL 0
#define BUZZER_TIMER_13_BIT 13
#define BUZZER_BASE_FREQ 5000

My_ToneTypeDef tones_5beep[] = {
    {4000, 8}, // Higher frequency and slightly longer duration
    {0, 10},   // Pause
    {3000, 6}, // Lower frequency
    {0, 10},   // Pause
    {4000, 8}, // Higher frequency and slightly longer duration
    {0, 10},   // Pause
    {3000, 6}, // Lower frequency
    {0, 8},    // Shorter pause
    {4000, 8}, // Higher frequency and slightly longer duration
    {0, 0}};

My_ToneTypeDef single_beep[] = {
    {1500, 3}, // Higher frequency and slightly longer duration
    {0, 0}};

My_ToneTypeDef tones_2_beep[] = {
    {1200, 6}, // Lower frequency
    {0, 3},    // Short pause
    {1200, 6}, // Lower frequency
    {0, 9},    // Longer pause
    {0, 0}};

My_ToneTypeDef tones_SMB[] = {
    {2637, 18}, {0, 9}, {2637, 9}, {0, 9}, {2093, 9}, {2637, 9}, {0, 9}, {3136, 9}, {0, 27}, {1586, 9}, {0, 27}, {2093, 9}, {0, 18}, {1586, 9}, {0, 18}, {1319, 9}, {0, 18}, {1760, 9}, {0, 9}, {1976, 9}, {0, 9}, {1865, 9}, {1760, 9}, {0, 9}, {1586, 12}, {2637, 12}, {3136, 12}, {3520, 9}, {0, 9}, {2794, 9}, {3136, 9}, {0, 9}, {2637, 9}, {0, 9}, {2093, 9}, {2349, 9}, {1976, 9}, {0, 18}, {2093, 9}, {0, 18}, {1586, 9}, {0, 18}, {1319, 9}, {0, 18}, {1760, 9}, {0, 9}, {1976, 9}, {0, 9}, {1865, 9}, {1760, 9}, {0, 9}, {1586, 12}, {2637, 12}, {3136, 12}, {3520, 9}, {0, 9}, {2794, 9}, {3136, 9}, {0, 9}, {2637, 9}, {0, 9}, {2093, 9}, {2349, 9}, {1976, 9}, {0, 0}};

My_ToneTypeDef tones_startup[] = {
    {2000, 3}, {0, 3}, {3000, 3}, {0, 3}, {4000, 3}, {0, 3}, {1200, 4}, {0, 6}, {4500, 6}, {0, 0}};

My_ToneTypeDef tones_turnoff[] = {
    {2000, 3}, {0, 3}, {3000, 3}, {0, 3}, {4000, 3}, {0, 3}, {1200, 4}, {0, 6}, {4500, 6}, {0, 0}};

My_ToneTypeDef tones_3beep[] = {
    {4000, 3}, {0, 10}, {1000, 6}, {0, 10}, {4000, 3}, {0, 0}};

My_ToneTypeDef *current_tone;
unsigned int note_index = 0;

unsigned long current_buzzer_time = 0;
unsigned long previous_buzzer_time = 0;
int turn_off = 1;

void buzzer_play_a_tone(My_ToneTypeDef *input_tone)
{
    current_tone = input_tone;
    note_index = 0;
    turn_off = false;

    current_buzzer_time = millis();
    previous_buzzer_time = current_buzzer_time;
    tone(BUZZER_PIN, current_tone[note_index].frequency, 100);
}

typedef enum
{
    BUZZER_TOGGLE_1S,
    BUZZER_PLAY_A_TONE,
    BUZZER_OFF
} BUZZER_STATE;

BUZZER_STATE buzzer_state = BUZZER_PLAY_A_TONE;

unsigned long buzzer_currentMillis = 0;
unsigned long buzzer_previousMillis = 0;
uint8_t buzzer_count = 0;
uint8_t buzzer_toggle = 1;

void buzzer_process(void)
{
    switch (buzzer_state)
    {
    case BUZZER_OFF:
        noTone(BUZZER_PIN);
        break;

    case BUZZER_PLAY_A_TONE:
        break;

    case BUZZER_TOGGLE_1S:
        buzzer_currentMillis = millis();
        if (buzzer_currentMillis - buzzer_previousMillis >= 1000)
        {
            buzzer_previousMillis = buzzer_currentMillis;
            buzzer_play_a_tone(single_beep);
        }
        break;

    default:
        buzzer_state = BUZZER_PLAY_A_TONE;
        break;
    }
}

void buzzer_play_a_tone_process(void)
{
    if (current_tone == NULL)
    {
        return;
    }

    if (current_tone[note_index].frequency != 0 || current_tone[note_index].duration != 0)
    {
        current_buzzer_time = millis();
        if (current_buzzer_time - previous_buzzer_time >= current_tone[note_index].duration * 12)
        {
            previous_buzzer_time = current_buzzer_time;
            tone(BUZZER_PIN, current_tone[note_index].frequency);
            note_index++;
        }
    }
    else
    {
        noTone(BUZZER_PIN);
    }
}

void buzzer_task(void *pvParameters)
{
    while (1)
    {
        buzzer_process();
        buzzer_play_a_tone_process();
        vTaskDelay(pdMS_TO_TICKS(50));
    }
}

TaskHandle_t buzzerTaskHandle = NULL;

void buzzer_init()
{
    note_index = 0;
    pinMode(BUZZER_PIN, OUTPUT);
    digitalWrite(BUZZER_PIN, LOW);

    ledcSetup(BUZZER_CHANNEL, BUZZER_BASE_FREQ, BUZZER_TIMER_13_BIT);
    ledcAttachPin(BUZZER_PIN, BUZZER_CHANNEL);
    ledcWrite(BUZZER_CHANNEL, 0);

    xTaskCreate(buzzer_task, "Buzzer Task", 2048, NULL, 2, &buzzerTaskHandle);

    sprintf(serial_buffer, "%-10s %-15s", "BUZZER:", "Initialized");
    Serial.println(serial_buffer);
}

void buzzer_set_play_a_tone()
{
    buzzer_state = BUZZER_PLAY_A_TONE;
}

void buzzer_toggle_1s()
{
    buzzer_state = BUZZER_TOGGLE_1S;
}

void buzzer_off()
{
    buzzer_state = BUZZER_OFF;
}