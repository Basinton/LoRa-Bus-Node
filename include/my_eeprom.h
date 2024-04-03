#ifndef __EEPROM_H_
#define __EEPROM_H_

/* Includes ------------------------------------------------------------------*/

/* Define --------------------------------------------------------------------*/

/* Variables -----------------------------------------------------------------*/

/* Functions -----------------------------------------------------------------*/
template <class T>
int EEPROM_writeAnything(int ee, const T &value);

template <class T>
int EEPROM_readAnything(int ee, T &value);

#endif