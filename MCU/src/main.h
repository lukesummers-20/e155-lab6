/**
    Main Header: Contains general defines and selected portions of CMSIS files
    @file main.h
    @author Josh Brake
    @version 1.0 10/7/2020
*/

#ifndef MAIN_H
#define MAIN_H

#include "../lib/STM32L432KC.h"

#define LED_PIN PB3 // LED pin for blinking on Port B pin 3
#define BUFF_LEN 32
#define SCK_PIN PA5
#define COPI_PIN PA12
#define CIPO_PIN PA6
#define CE_PIN PB1
#define LSB_ADR  0b00000001
#define MSB_ADR  0b00000010
#define CFGR_ADR 0b10000000

#endif // MAIN_H