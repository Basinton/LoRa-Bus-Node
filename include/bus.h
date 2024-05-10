#ifndef __BUS_H_
#define __BUS_H_

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Define --------------------------------------------------------------------*/
#define CURRENT_BUS_ID 0
#define BUS_LIST 5

const uint16_t BUS_ADDRESS_LIST[BUS_LIST] = {0x0001, 0x0010, 0x0011, 0x0100, 0x0101}; // Node's Addresses
const uint16_t BUS_CHANEL_LIST[BUS_LIST] = {11, 22, 33, 44, 55};                      // Node's Channels
const uint16_t BUS_ADDRESS = BUS_ADDRESS_LIST[CURRENT_BUS_ID];
const uint16_t BUS_CHANNEL = BUS_CHANEL_LIST[CURRENT_BUS_ID];

const uint16_t GATEWAY_ADDRESS = 0x0001;
const uint16_t GATEWAY_CHANNEL = 1;

const uint8_t BUS_ROUTE = 50;

#define ID_INDEX 1
#define ADDRESS_HI_INDEX 2
#define ADDRESS_LO_INDEX 3
#define STATE_INDEX 4
#define BUS_NUMBER_INDEX 5
#define BUS_DIRECTION_INDEX 6
#define STATION_ID_INDEX 7

typedef enum
{
    ERROR_TIMEOUT = 50,
    INIT = 0,
    WAITING = 1,
    REQUEST_TO_STATION = 2,
    STATION_NOTIFY_ACCEPT_TO_BOARD = 3,
    REQUEST_TO_BUS = 4,
    STATION_NOTIFY_BUS_ACCEPT_TO_BOARD = 5,
    BUS_ACCEPT = 6,
    STATION_NOTIFY_BUS_PASS_TO_BOARD = 7,
    BUS_PASS = 8,
    STATION_NOTIFY_DRIVER_CANCEL_TO_BOARD = 9,
    DRIVER_CANCEL = 10,
    BOARD_NOTIFY_PASSENGER_CANCEL_TO_STATION = 11,
    PASSENGER_CANCEL = 12,
    FINISHED = 13
} SYSTEM_STATE;

typedef enum
{
    BUS_UNKNOWN = -1,
    BUS_00,
    BUS_01,
    BUS_02,
    BUS_03,
    BUS_04,
    BUS_COUNT
} BUS_ID;

extern BUS_ID busID;

struct BUS
{
    u8_t busRoute;
    String busDriverName;

    float busLat;
    float busLong;
    double busSpeed;

    u8_t busDirection;
    u8_t nowBusStop;
    u8_t preBusStop;
};

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

    String stationName;
};

// const Stations STATIONS[] = {
//     {0, 0, "Không xác định"},
//     {10.773363, 106.656001, "VP1"},
//     {10.773072, 106.655213, "Vi tính Tài Phát"},
//     // {10.772997, 106.657328, "BK Computer"},
//     // {10.771192, 106.657806, "Trạm bus LTK"},
//     // {10.770616, 106.656731, "Bánh mì HN"},
//     // {10.771142, 106.653218, "Ngã 3 Lữ Gia"},
//     // {10.773117, 106.652984, "Mixue"},
//     // {10.774278, 106.654749, "Hẻm 281"},
//     // {10.774872, 106.656626, "Petrolimex"},
//     {10.772716, 106.654931, "VP2"}};

// LoRa HK232 testing stations GPS data for BUS_50
// const Stations STATIONS[] = {
//     {0, 0, "Không xác định"},
//     {10.772716, 106.654931, "VP2"},
//     {10.773365, 106.661062, "Đại học Bách Khoa (cổng sau)"},
//     {10.771294, 106.659070, "Bệnh viện Trưng Vương"},
//     {10.772604, 106.657698, "Đại học Bách Khoa"},
//     {10.776469, 106.656531, "Bưu Điện Phú Thọ"},
//     {10.778741, 106.655909, "Ngã ba Thành Thái"},
//     {10.781072, 106.655288, "Siêu thị Nguyễn Kim - CMC Tân Bình"},
//     {10.784002, 106.654430, "Cây xăng Đôi"},
//     {10.787758, 106.653368, "Chợ Tân Bình"}};

const Stations STATIONS[] = {
    {0, 0, "Không xác định"},
    {10.772716, 106.654931, "VP2"},
    {10.772604, 106.657698, "Đại học Bách Khoa"},
    {10.771294, 106.659070, "Bệnh viện Trưng Vương"},
    {10.773365, 106.661062, "Đại học Bách Khoa (cổng sau)"}
};
// {10.776469, 106.656531, "Bưu Điện Phú Thọ"},
// {10.778741, 106.655909, "Ngã ba Thành Thái"},
// {10.781072, 106.655288, "Siêu thị Nguyễn Kim - CMC Tân Bình"},
// {10.784002, 106.654430, "Cây xăng Đôi"},
// {10.787758, 106.653368, "Chợ Tân Bình"}};

#define STATIONS_N sizeof(STATIONS) / sizeof(STATIONS[0])

enum BusDirection
{
    NOT_KNOWN,
    START_TO_END,
    END_TO_START
};

struct dirDisplay
{
    u8_t dirValue;
    String dirName;
};

const dirDisplay direction[] = {
    {NOT_KNOWN, "Không xác định"},
    {START_TO_END, "Đầu đến cuối"},
    {END_TO_START, "Cuối đến đầu"}};

enum BUS_MODE
{
    TESTING,
    NORMAL
};

// Define a constant for the maximum distance to consider a bus stop as reached (in meters)
const float MAX_DISTANCE_TO_BUS_STOP = 10.0;

/* Variables -----------------------------------------------------------------*/
extern BUS myBus;

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
void updateBusDirection();
void updateBusStopsList();
void busUpdateLocationToStation();

void busAckToStation(SYSTEM_STATE state);
void bus_ack_debuger(void);
void bus_fsm_reset_state(SYSTEM_STATE state);
void bus_fsm(void);
void busCancel(void);

#endif