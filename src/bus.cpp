/* Includes ------------------------------------------------------------------*/
#include "bus.h"

/* Define --------------------------------------------------------------------*/

/* Variables -----------------------------------------------------------------*/
TaskHandle_t busTaskUpdateHandle = NULL;
TaskHandle_t busTaskHandle = NULL;

uint8_t busLoraMessage[20] = {0};

BUS_MODE busMode = TESTING;

BUS_ID busID = BUS_00;

uint8_t errorCount[BUS_COUNT] = {0};

// Update bus direction
int busRoute = BUS_ROUTE;
char *busDriverName = "Nguyễn Văn A";
int busDirection = NOT_KNOWN;
int nowBusStop = -1;
int preBusStop = -1;

// Station request
SYSTEM_STATE busState = INIT;
uint8_t busTimout = 0;
STATION_REQUEST station_request = {0};

uint8_t isThereRequest = 0;
uint8_t isFinishedAck = 0;
uint8_t isCancelAck = 0;

uint8_t iWantToAcceptBus = 0;
uint8_t isPassengerCancel = 0;
uint8_t isStationReAckBusAccept = 0;
uint8_t isStationReAckPassengerCancel = 0;

/* Functions -----------------------------------------------------------------*/

void updateBusDirection()
{
    static float _distanceToStart = 0.0;
    static float _distanceToEnd = 0.0;

    if (busDirection == NOT_KNOWN)
    {
        // Serial.println("bus: \t [direction] now known");
        if (gps.location.lat() == 0 || gps.location.lng() == 0)
        {
            return;
        }

        // Calculate the distance to the start and end bus stops
        _distanceToStart = TinyGPSPlus::distanceBetween(gps.location.lat(), gps.location.lng(), STATIONS[0].lat, STATIONS[0].lng);
        _distanceToEnd = TinyGPSPlus::distanceBetween(gps.location.lat(), gps.location.lng(), STATIONS[STATIONS_N - 1].lat, STATIONS[STATIONS_N - 1].lng);

        if (_distanceToStart < _distanceToEnd)
        {
            busDirection = START_TO_END;
        }
        else
        {
            busDirection = END_TO_START;
        }
    }
    else
    {
        if (nowBusStop != -1 && preBusStop != -1)
        {
            // Check if the bus has reached the start or end station
            if (nowBusStop == 0 && (preBusStop > 0 && preBusStop <= STATIONS_N - 1))
            {
                nowBusStop = 0;
                preBusStop = -1;
                busDirection = START_TO_END;
            }
            else if (nowBusStop == STATIONS_N - 1 && (preBusStop >= 0 && preBusStop < STATIONS_N - 1))
            {
                nowBusStop = STATIONS_N - 1;
                preBusStop = -1;
                busDirection = END_TO_START;
            }
            else
            {
                if (nowBusStop > preBusStop)
                {
                    busDirection = START_TO_END;
                }
                else
                {
                    busDirection = END_TO_START;
                }
            }
        }
    }
}

void updateBusStopsList()
{
    static float _minDistance = 0.0;
    static int _nearestBusStop = 0;
    static float distance = 0.0;

    // 1. Find the nearest bus stop.
    _minDistance = MAX_DISTANCE_TO_BUS_STOP;
    _nearestBusStop = -1;

    for (int i = 0; i < STATIONS_N; i++)
    {
        float _distance = TinyGPSPlus::distanceBetween(gps.location.lat(), gps.location.lng(), STATIONS[i].lat, STATIONS[i].lng);
        if (_distance < _minDistance)
        {
            _minDistance = _distance;
            _nearestBusStop = i;
        }
    }

    // 2. If nearest bus stop distance <= MAX_DISTANCE_TO_BUS_STOP, update bus_stop.
    if (_nearestBusStop >= 0 && _minDistance <= MAX_DISTANCE_TO_BUS_STOP)
    {
        if (nowBusStop == -1 && preBusStop == -1)
        {
            nowBusStop = _nearestBusStop;
        }
        else if (_nearestBusStop != nowBusStop)
        {
            preBusStop = nowBusStop;
            nowBusStop = _nearestBusStop;
        }
    }
}

void busTaskUpdate(void *pvParameters)
{
    (void)pvParameters;

    while (1)
    {
        updateBusDirection();
        updateBusStopsList();

        // Serial.printf("bus: \t [dir=%d][now=%d][pre=%d]\n", busDirection, nowBusStop, preBusStop);

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void busAckToStation(SYSTEM_STATE state)
{
    busLoraMessage[0] = 0xFF;

    busLoraMessage[1] = busID;
    busLoraMessage[2] = station_request.id;

    busLoraMessage[3] = (BUS_ADDRESS >> 8) & 0xFF; // Bus Address HIGH
    busLoraMessage[4] = BUS_ADDRESS & 0xFF;        // Bus Address LOW

    busLoraMessage[5] = state;

    busLoraMessage[6] = checkSum(busLoraMessage, LORA_PACKAGE_SIZE_SEND);

    // e32ttl100.sendBroadcastFixedMessage(GATEWAY_CHANNEL, busLoraMessage, LORA_PACKAGE_SIZE_SEND + 1);
    e32ttl100.sendFixedMessage(station_request.addressHI, station_request.addressLO, GATEWAY_CHANNEL, busLoraMessage, LORA_PACKAGE_SIZE_SEND + 1);
}

void bus_ack_debuger(void)
{
    if (isStationReAckBusAccept)
    {
        isStationReAckBusAccept = 0;

        busAckToStation(BUS_ACCEPT);
        Serial.println("lora: \t [ack debug] bus accecpt");
    }

    if (isStationReAckPassengerCancel)
    {
        isStationReAckPassengerCancel = 0;

        busAckToStation(PASSENGER_CANCEL);
        Serial.println("lora: \t [ack debug] passenger cancel");
    }
}

void bus_fsm_reset_state(SYSTEM_STATE state)
{
    switch (state)
    {
    case WAITING:
        isThereRequest = 0;
        isFinishedAck = 0;
        isCancelAck = 0;

        led_off();
        buzzer_off();

        Serial.println("bus: \t [fsm] waiting");
        break;

    case BUS_ACCEPT:
        busAckToStation(BUS_ACCEPT);

        led_toggle_1s();
        buzzer_toggle_1s();

        Serial.printf("bus: \t [fsm] accept (id=%d, address=%d)\n", station_request.id, station_request.id);
        break;

    case BUS_PASS:
        busAckToStation(BUS_PASS);

        led_off();
        buzzer_off();

        busTimout = 40;

        Serial.println("bus: \t [fsm] finish");
        break;

    case DRIVER_CANCEL:
        busAckToStation(DRIVER_CANCEL);

        busTimout = 40;

        buzzer_off();

        Serial.println("bus: \t [fsm] bus cancel");
        break;

    case PASSENGER_CANCEL:
        busAckToStation(PASSENGER_CANCEL);

        busTimout = 40;

        Serial.println("bus: \t [fsm] passenger cancel");
        break;

    case ERROR_TIMEOUT:
        sprintf(serial_buffer, "%-10s %-15s", "BUS:", "Error TIMEOUT");
        Serial.println(serial_buffer);
        break;

    default:
        break;
    }
}

void bus_fsm(void)
{
    static float _distanceToRequestStation = 0;

    switch (busState)
    {
    case INIT:
        sprintf(serial_buffer, "%-10s %-15s", "BUS_FSM:", "Initialized");
        Serial.println(serial_buffer);
        bus_fsm_reset_state(WAITING);
        busState = WAITING;
        break;

    case WAITING:
        if (isThereRequest)
        {
            // reset request flag
            isThereRequest = 0;
            // change fsm state to BUS_ACCEPT
            bus_fsm_reset_state(BUS_ACCEPT);
            busState = BUS_ACCEPT;
        }
        break;

    case BUS_ACCEPT:
        if (nowBusStop == station_request.stationID)
        {
            bus_fsm_reset_state(BUS_PASS);
            busState = BUS_PASS;
        }

        if (keyCode == 60)
        {
            buzzer_set_play_a_tone();
            buzzer_play_a_tone(tones_3beep);
        }
        // if (keyCode == 120)
        // {
        //     buzzer_play_a_tone(tones_turnoff);
        // }

        if (keyCode == 0)
        {
            if (button_keycode_release() >= 60)
            {
                keyTimeBeforeReleasing = 0;

                bus_fsm_reset_state(DRIVER_CANCEL);
                busState = DRIVER_CANCEL;
            }
        }

        // else if (button_keycode_release() >= 120)
        // {
        //     keyTimeBeforeReleasing = 0;

        //     bus_fsm_reset_state(BUS_PASS);
        //     busState = BUS_PASS;
        // }
        // }

        break;

    case BUS_PASS:
        _distanceToRequestStation = TinyGPSPlus::distanceBetween(gps.location.lat(), gps.location.lng(), STATIONS[station_request.stationID].lat, STATIONS[station_request.stationID].lng);

        // if (isFinishedAck)
        if (isFinishedAck || _distanceToRequestStation >= 500)
        {
            isFinishedAck = 0;

            bus_fsm_reset_state(WAITING);
            busState = WAITING;
        }
        else if (--busTimout == 0)
        {
            if (errorCount[busID] >= 20)
            {
                errorCount[busID] = 0;
                bus_fsm_reset_state(ERROR_TIMEOUT);
                busState = ERROR_TIMEOUT;
            }
            errorCount[busID]++;

            bus_fsm_reset_state(BUS_PASS);
            busState = BUS_PASS;
        }

        break;

    case DRIVER_CANCEL:
        if (isCancelAck)
        {
            isCancelAck = 0;

            bus_fsm_reset_state(WAITING);
            busState = WAITING;
        }
        else if (--busTimout == 0)
        {
            if (errorCount[busID] >= 20)
            {
                errorCount[busID] = 0;
                bus_fsm_reset_state(ERROR_TIMEOUT);
                busState = ERROR_TIMEOUT;
            }
            errorCount[busID]++;

            bus_fsm_reset_state(DRIVER_CANCEL);
            busState = DRIVER_CANCEL;
        }

        break;

    case PASSENGER_CANCEL:
        bus_fsm_reset_state(WAITING);
        busState = WAITING;
        break;

    case ERROR_TIMEOUT:
        bus_fsm_reset_state(WAITING);
        busState = WAITING;
        break;

    default:
        busState = INIT;
        break;
    }
}

void busCancel(void)
{
    if (isPassengerCancel)
    {
        isPassengerCancel = 0;

        bus_fsm_reset_state(PASSENGER_CANCEL);
        busState = PASSENGER_CANCEL;
    }
}

void busTask(void *pvParameters)
{
    (void)pvParameters;

    while (1)
    {
        busCancel();
        bus_ack_debuger();
        bus_fsm();

        vTaskDelay(pdMS_TO_TICKS(50));
    }
}

void bus_init()
{
    xTaskCreate(busTaskUpdate, "Bus Update", 8192, NULL, 2, &busTaskUpdateHandle);
    xTaskCreate(busTask, "Main process of Bus", 8192, NULL, 3, &busTaskHandle);

    sprintf(serial_buffer, "%-10s %-15s", "BUS:", "Initialized");
    Serial.println(serial_buffer);
}