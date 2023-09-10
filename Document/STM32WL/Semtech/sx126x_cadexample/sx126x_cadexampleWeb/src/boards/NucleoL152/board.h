/*
  ______                              _
 / _____)             _              | |
( (____  _____ ____ _| |_ _____  ____| |__
 \____ \| ___ |    (_   _) ___ |/ ___)  _ \
 _____) ) ____| | | || |_| ____( (___| | | |
(______/|_____)_|_|_| \__)_____)\____)_| |_|
    (C)2013 Semtech

Description: Target board general functions implementation

License: Revised BSD License, see LICENSE.TXT file include in the project

Maintainer: Miguel Luis and Gregory Cristian
*/
#ifndef __BOARD_H__
#define __BOARD_H__

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "stm32l1xx.h"
#include "stm32l1xx_hal.h"
#include "utilities.h"
#include "timer.h"
#include "delay.h"
#include "gpio.h"
#include "spi.h"
#include "uart.h"
#include "radio.h"
#include "sx126x/sx126x.h"
#include "sx126x-board.h"
#include "rtc-board.h"

#include "uart-board.h"

/*!
 * Generic definition
 */
#ifndef SUCCESS
#define SUCCESS                                     1
#endif

#ifndef FAIL
#define FAIL                                        0
#endif

/*!
 * Board MCU pins definitions
 */
#define RADIO_RESET                                 PA_0

#define RADIO_MOSI                                  PA_7
#define RADIO_MISO                                  PA_6
#define RADIO_SCLK                                  PA_5
#define RADIO_NSS                                   PA_8

#define RADIO_BUSY                                  PB_3
#define RADIO_DIO_1                                 PB_4

#define ANT_SWITCH_POWER                            PA_9

#define OSC_LSE_IN                                  PC_14
#define OSC_LSE_OUT                                 PC_15

#define OSC_HSE_IN                                  PH_0
#define OSC_HSE_OUT                                 PH_1

#define SWCLK                                       PA_14
#define SWDAT                                       PA_13

#define I2C_SCL                                     PB_8
#define I2C_SDA                                     PB_9

#define UART_TX                                     PA_2
#define UART_RX                                     PA_3

#define LED_1                                       PC_1
#define LED_2                                       PC_0

#define FR                                          PA_1
#define OPT                                         PB_0
#define DEVICE_SEL                                  PA_4

/*!
 * LED GPIO pins objects
 */
extern Gpio_t Led1;
extern Gpio_t Led2;

/*!
 * MCU objects
 */
extern Uart_t Uart2;

/*!
 * Possible power sources
 */
enum BoardPowerSources
{
    USB_POWER = 0,
    BATTERY_POWER,
};

/*!
 * \brief Disable interrupts
 *
 * \remark IRQ nesting is managed
 */
void BoardDisableIrq( void );

/*!
 * \brief Enable interrupts
 *
 * \remark IRQ nesting is managed
 */
void BoardEnableIrq( void );

/*!
 * \brief Initializes the target board peripherals.
 */
void BoardInitMcu( void );

/*!
 * \brief Initializes the boards peripherals.
 */
void BoardInitPeriph( void );

/*!
 * \brief De-initializes the target board peripherals to decrease power
 *        consumption.
 */
void BoardDeInitMcu( void );

/*!
 * \brief Measure the Battery voltage
 *
 * \retval value  battery voltage in volts
 */
uint32_t BoardGetBatteryVoltage( void );

/*!
 * \brief Get the current battery level
 *
 * \retval value  battery level [  0: USB,
 *                                 1: Min level,
 *                                 x: level
 *                               254: fully charged,
 *                               255: Error]
 */
uint8_t BoardGetBatteryLevel( void );

/*!
 * Returns a pseudo random seed generated using the MCU Unique ID
 *
 * \retval seed Generated pseudo random seed
 */
uint32_t BoardGetRandomSeed( void );

/*!
 * \brief Gets the board 64 bits unique ID
 *
 * \param [IN] id Pointer to an array that will contain the Unique ID
 */
void BoardGetUniqueId( uint8_t *id );

/*!
 * \brief Get the board power source
 *
 * \retval value  power source [0: USB_POWER, 1: BATTERY_POWER]
 */
uint8_t GetBoardPowerSource( void );

#endif // __BOARD_H__
