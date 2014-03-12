/**
 * \addtogroup nrf51822
 * @{
 * \defgroup nrf51-clock Clock implementation for nrf51822 SoC
 * 			This module include the initialization of the 16 MHz high
 * 			frequency clock, 32.768 kHz LF clock and RTC1. Also all the
 * 			implementations of functions required by \ref nrf51-contiki-clock
 * 			module are defined here.
 *
 * @{
 * \file
 * nrf51822 architecture specific declarations for Contiki's Rtimer
 * \author prithvi
 */

#ifndef CLOCK_NRF_H_
#define CLOCK_NRF_H_

#include "nrf.h"
#include "clock.h"
#include "leds.h"
#include "nrf-delay.h"
#include "nrf-gpio.h"
#include "simple-uart.h"

/** LF Clock frequency in Hertz, constant. */
#define LFCLK_FREQUENCY           (32768UL)

/** \brief Prescalar value required for RTC initialization.
 *  Based on the formula f = LFCLK/(prescaler + 1)
 */
#define COUNTER_PRESCALER         ((LFCLK_FREQUENCY/CLOCK_SECOND) - 1)

/** Unsigned int to store the number of seconds passed, required by Contiki */
static volatile uint32_t current_seconds = 0;

#if TICKLESS == true
/** Unsigned int to store the number of times the RTC has overflowed to calculate \ref current_seconds */
static volatile uint32_t seconds_ovr = 0;
/** Variable to store the seconds offset if \ref clock_set_seconds is called */
static volatile uint32_t seconds_offset = 0;
#else
/** Variable to store the number of TICK of the clock */
static volatile clock_time_t current_clock = 0;
/** Variable to store the number of ticks remaining to increment \ref current_seconds */
static volatile unsigned int second_countdown = CLOCK_SECOND;
#endif

void rtc_init(void);
void lfclk_init(void);
void lfclk_deinit(void);
void hfclk_xtal_init(void);
void hfclk_xtal_deinit(void);
clock_time_t nrf_clock_time(void);
unsigned long nrf_clock_seconds(void);
void nrf_clock_set_seconds(uint32_t sec);
void nrf_clock_update_expiration_time(clock_time_t expiration_time);

#endif /* CLOCK_NRF_H_ */

/**
 * @}
 * @}
 */
