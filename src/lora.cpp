/* Includes ------------------------------------------------------------------*/
#include "lora.h"
#include "bus.h"
#include "gps.h"
#include "button.h"

#include "Arduino.h"
#include "LoRa_E32.h"

/* Define --------------------------------------------------------------------*/

/* Variables -----------------------------------------------------------------*/
uint8_t lora_receive[1000] = {0};
uint32_t lora_receive_cnt  = 0;

LoRa_E32 e32ttl100(&Serial2, PIN_AUX, PIN_M0, PIN_M1, UART_BPS_RATE_9600);

TaskHandle_t loraTaskHandle = NULL;

/* Functions -----------------------------------------------------------------*/
void lora_task(void *pvParameters)
{
    while (1)
    {
        lora_process();

        vTaskDelay(pdMS_TO_TICKS(50));
    }
}

void lora_init(void)
{
    e32ttl100.begin();
    setConfig(BUS_ADDRESS, BUS_CHANNEL, AIR_DATA_RATE_000_03, POWER_20);

    xTaskCreate(lora_task, "lora task", 8192, NULL, configMAX_PRIORITIES, &loraTaskHandle);

    Serial.println("lora: \t [init]");
}

uint8_t checkSum(uint8_t *message, int size)
{
    uint8_t checksum = 0;
    for (uint8_t i = 1; i < size; i++)
        checksum = checksum + message[i];
    return checksum;
}

void checkDataReceive(void)
{
    static uint8_t status = 0;
    static uint8_t checksum;
    static uint32_t startDataReach = 0;

    static uint32_t sizeOfData = lora_receive_cnt - LORA_PACKAGE_SIZE_RECIEVE;

    static uint8_t _stationID = 0;

    for (uint32_t i = 0; i < lora_receive_cnt; i++)
    {
        if (i >= sizeOfData)
        {
            break;
        }

        if (lora_receive[i] == 0xff)
        {
            checksum = checkSum(&lora_receive[i], LORA_PACKAGE_SIZE_RECIEVE);
            if (checksum == lora_receive[i + LORA_PACKAGE_SIZE_RECIEVE])
            {
                switch (lora_receive[i + STATE_INDEX])
                {
                    case REQUEST_TO_BUS:
                        if (busState == WAITING)
                        {
                            // 0. Get ID of Station (temp)
                            _stationID = lora_receive[i + STATION_ID_INDEX];

                            // 1. Check condition to accept
                            static uint8_t _conditionBusAccept = 0;

                            // _conditionBusAccept = (lora_receive[i + BUS_NUMBER_INDEX] == busNumber) && (lora_receive[i + BUS_DIRECTION_INDEX] == busDirection);
                            // _conditionBusAccept = _conditionBusAccept && (_stationID != 0 && _stationID != STATIONS_N - 1) || button_keycode() == 20;
                            // _conditionBusAccept = (_conditionBusAccept && (_stationID == nowBusStop + 1)) || button_keycode() == 20;

                            _conditionBusAccept = iWantToAcceptBus;
                            iWantToAcceptBus    = 0;

                            // 2. If ok, accept
                            if (_conditionBusAccept)
                            {
                                station_request.id        = lora_receive[i + ID_INDEX];
                                station_request.addressHI = lora_receive[i + ADDRESS_HI_INDEX];
                                station_request.addressLO = lora_receive[i + ADDRESS_LO_INDEX];
                                station_request.stationID = lora_receive[i + STATION_ID_INDEX];

                                isThereRequest = 1;
                            }
                            else
                            {
                                // Debugging messages
                                if (lora_receive[i + BUS_NUMBER_INDEX] != busNumber)
                                    Serial.println("lora:\t[bus] wrong bus number");
                                if (lora_receive[i + BUS_DIRECTION_INDEX] != busDirection)
                                    Serial.println("lora:\t[bus] wrong direction");
                                if (_stationID != nowBusStop + 1)
                                    Serial.println("lora:\t[bus] is not next station");
                                if (_stationID == 0)
                                    Serial.println("lora:\t[bus] first station");
                                if (_stationID == STATIONS_N - 1)
                                    Serial.println("lora:\t[bus] last station");
                            }
                        }
                        else
                        {
                            isStationReAckBusAccept = 1;
                        }
                        break;

                    case BUS_PASS:
                        if (busState == BUS_PASS)
                        {
                            isFinishedAck = 1;
                        }

                        break;

                    case DRIVER_CANCEL:
                        if (busState == DRIVER_CANCEL)
                        {
                            isCancelAck = 1;
                        }

                        break;

                    case PASSENGER_CANCEL:
                        if (busState != INIT && busState != WAITING && busState != DRIVER_CANCEL && busState != PASSENGER_CANCEL)
                        {
                            isPassengerCancel = 1;
                        }
                        else
                        {
                            isStationReAckPassengerCancel = 1;
                        }

                        break;

                    default:
                        break;
                }

                startDataReach = i + LORA_PACKAGE_SIZE_RECIEVE + 1;
                i              = i + LORA_PACKAGE_SIZE_RECIEVE;
            }
            else
            {
                // Serial.println("lost");
                // startDataReach = i + LORA_PACKAGE_SIZE_RECIEVE + 1;
                // i              = i + LORA_PACKAGE_SIZE_RECIEVE;
            }
        }
    }

    if (lora_receive_cnt < startDataReach)
        return;

    for (uint32_t i = 0; i < lora_receive_cnt - startDataReach; i++)
    {
        if (i >= (lora_receive_cnt - startDataReach))
            break;
        lora_receive[i] = lora_receive[startDataReach + i];
    }
    lora_receive_cnt = lora_receive_cnt - startDataReach;

    memset(&lora_receive[lora_receive_cnt], 0x00, 1000 - lora_receive_cnt);
}

void lora_process(void)
{
    static uint32_t _checkNoData = 0;

    if (e32ttl100.available() > 1)
    {
        _checkNoData         = 0;
        ResponseContainer rs = e32ttl100.receiveMessage();

        for (uint32_t i = 0; i < rs.data.length(); i++)
        {
            lora_receive[lora_receive_cnt] = rs.data[i];
            lora_receive_cnt               = (lora_receive_cnt + 1) % 1000;
        }
    }
    else
        _checkNoData = _checkNoData + 1;

    if (_checkNoData > 1)
    {
        lora_receive_cnt = 0;
        _checkNoData     = 0;
    }

    if (lora_receive_cnt > LORA_PACKAGE_SIZE_RECIEVE)
    {
        Serial.printf("lora: [%d] ", lora_receive_cnt);
        for (size_t i = 0; i < lora_receive_cnt; i++)
        {
            printf("%02X ", lora_receive[i]);
        }
        printf("\n");

        checkDataReceive();
    }
}

void accessModeConfig(void)
{
    digitalWrite(PIN_M0, HIGH);
    digitalWrite(PIN_M1, HIGH);
}

void accessModeTransmit(void)
{
    digitalWrite(PIN_M0, LOW);
    digitalWrite(PIN_M1, LOW);
}

void setConfig(uint16_t address, uint16_t channel, uint8_t airRate, uint8_t power)
{
    accessModeConfig();
    delay(10);
    ResponseStructContainer c;
    c = e32ttl100.getConfiguration();
    // It's important get configuration pointer before all other operation
    Configuration configuration = *(Configuration *)c.data;

    // Serial.println(c.status.getResponseDescription());
    // Serial.println(c.status.code);
    // printParameters(configuration);

    // Configuration configuration;
    configuration.ADDL = address & 0xff;
    configuration.ADDH = (address >> 8) & 0xff;
    configuration.CHAN = channel;

    configuration.OPTION.fec                = FEC_1_ON;
    configuration.OPTION.fixedTransmission  = FT_FIXED_TRANSMISSION;
    configuration.OPTION.ioDriveMode        = IO_D_MODE_PUSH_PULLS_PULL_UPS;
    configuration.OPTION.transmissionPower  = power;
    configuration.OPTION.wirelessWakeupTime = WAKE_UP_250;

    configuration.SPED.airDataRate  = airRate;
    configuration.SPED.uartBaudRate = UART_BPS_9600;
    configuration.SPED.uartParity   = MODE_00_8N1;

    // Set configuration changed and set to not hold the configuration
    ResponseStatus rs = e32ttl100.setConfiguration(configuration, WRITE_CFG_PWR_DWN_SAVE);

    // Serial.println(rs.getResponseDescription());
    // Serial.println(rs.code);
    // printParameters(configuration);

    c.close();
    accessModeTransmit();
}

void printParameters(struct Configuration configuration)
{
    Serial.println("----------------------------------------");

    Serial.print(F("HEAD : "));
    Serial.print(configuration.HEAD, BIN);
    Serial.print(" ");
    Serial.print(configuration.HEAD, DEC);
    Serial.print(" ");
    Serial.println(configuration.HEAD, HEX);
    Serial.println(F(" "));
    Serial.print(F("AddH : "));
    Serial.println(configuration.ADDH, BIN);
    Serial.print(F("AddL : "));
    Serial.println(configuration.ADDL, BIN);
    Serial.print(F("Chan : "));
    Serial.print(configuration.CHAN, DEC);
    Serial.print(" -> ");
    Serial.println(configuration.getChannelDescription());
    Serial.println(F(" "));
    Serial.print(F("SpeedParityBit     : "));
    Serial.print(configuration.SPED.uartParity, BIN);
    Serial.print(" -> ");
    Serial.println(configuration.SPED.getUARTParityDescription());
    Serial.print(F("SpeedUARTDatte  : "));
    Serial.print(configuration.SPED.uartBaudRate, BIN);
    Serial.print(" -> ");
    Serial.println(configuration.SPED.getUARTBaudRate());
    Serial.print(F("SpeedAirDataRate   : "));
    Serial.print(configuration.SPED.airDataRate, BIN);
    Serial.print(" -> ");
    Serial.println(configuration.SPED.getAirDataRate());

    Serial.print(F("OptionTrans        : "));
    Serial.print(configuration.OPTION.fixedTransmission, BIN);
    Serial.print(" -> ");
    Serial.println(configuration.OPTION.getFixedTransmissionDescription());
    Serial.print(F("OptionPullup       : "));
    Serial.print(configuration.OPTION.ioDriveMode, BIN);
    Serial.print(" -> ");
    Serial.println(configuration.OPTION.getIODroveModeDescription());
    Serial.print(F("OptionWakeup       : "));
    Serial.print(configuration.OPTION.wirelessWakeupTime, BIN);
    Serial.print(" -> ");
    Serial.println(configuration.OPTION.getWirelessWakeUPTimeDescription());
    Serial.print(F("OptionFEC          : "));
    Serial.print(configuration.OPTION.fec, BIN);
    Serial.print(" -> ");
    Serial.println(configuration.OPTION.getFECDescription());
    Serial.print(F("OptionPower        : "));
    Serial.print(configuration.OPTION.transmissionPower, BIN);
    Serial.print(" -> ");
    Serial.println(configuration.OPTION.getTransmissionPowerDescription());

    Serial.println("----------------------------------------");
}