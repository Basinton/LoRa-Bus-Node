#pragma once
#ifndef __BUS_H_
#define __BUS_H_

/* Includes ------------------------------------------------------------------*/
#include <stdio.h>

/* Define --------------------------------------------------------------------*/
const uint16_t GATEWAY_ADDRESS = 0x0001;
const uint16_t BUS_ADDRESS     = 0x0800; // 0xFFFF / 2
const uint16_t GATEWAY_CHANNEL = 1;
const uint16_t BUS_CHANNEL     = 11;

const uint8_t BUS_NUMBER = 50;

#define ID_INDEX            1
#define ADDRESS_HI_INDEX    2
#define ADDRESS_LO_INDEX    3
#define STATE_INDEX         4
#define BUS_NUMBER_INDEX    5
#define BUS_DIRECTION_INDEX 6
#define STATION_ID_INDEX    7

typedef enum
{
    INIT                                     = 0,
    WAITING                                  = 1,
    REQUEST_TO_STATION                       = 2,
    STATION_NOTIFY_ACCEPT_TO_BOARD           = 3,
    REQUEST_TO_BUS                           = 4,
    STATION_NOTIFY_BUS_ACCEPT_TO_BOARD       = 5,
    BUS_ACCEPT                               = 6,
    STATION_NOTIFY_BUS_PASS_TO_BOARD         = 7,
    BUS_PASS                                 = 8,
    STATION_NOTIFY_DRIVER_CANCEL_TO_BOARD    = 9,
    DRIVER_CANCEL                            = 10,
    BOARD_NOTIFY_PASSENGER_CANCEL_TO_STATION = 11,
    PASSENGER_CANCEL                         = 12,
    FINISHED                                 = 13
} SYSTEM_STATE;

struct STATION_REQUEST
{
    uint8_t id;

    uint8_t addressHI;
    uint8_t addressLO;

    uint16_t stationID;
};

// Define a structure to represent a location
struct Stations
{
    float lat;
    float lng;
};

// Stations STATIONS[] = {
//     {10.773363, 106.656001},
//     {10.772997, 106.657328},
//     {10.771192, 106.657806},
//     {10.770616, 106.656731},
//     {10.771142, 106.653218},
//     {10.773117, 106.652984},
//     {10.774278, 106.654749},
//     {10.774872, 106.656626}};
const Stations STATIONS[] = {
    {10.772186, 106.652858},
    {10.773046, 106.655203},
    {10.773762, 106.657042}};

// #define STATIONS_N sizeof(STATIONS) / sizeof(STATIONS[0])
#define STATIONS_N 3

enum BusDirection
{
    NOT_KNOWN    = 2,
    START_TO_END = 1,
    END_TO_START = 0
};

enum BUS_MODE
{
    TESTING,
    NORMAL
};

// Define a constant for the maximum distance to consider a bus stop as reached (in meters)
const float MAX_DISTANCE_TO_BUS_STOP = 10.0;

/* Variables -----------------------------------------------------------------*/
extern int busNumber;
extern int busDirection;
extern int nowBusStop;
extern int preBusStop;

extern uint8_t isThereRequest;
extern uint8_t isFinishedAck;
extern uint8_t isCancelAck;

extern uint8_t iWantToAcceptBus;
extern uint8_t isPassengerCancel;
extern uint8_t isStationReAckBusAccept;
extern uint8_t isStationReAckPassengerCancel;

extern STATION_REQUEST station_request;

extern SYSTEM_STATE busState;

extern BUS_MODE busMode;

/* Functions -----------------------------------------------------------------*/
void bus_init(void);
void busAckToStation(SYSTEM_STATE state);

#endif