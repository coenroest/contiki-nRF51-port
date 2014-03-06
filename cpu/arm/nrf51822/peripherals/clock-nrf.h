/**
 * \file clock-nrf.h
 *
 *  Created on: 30-Jan-2014
 * \author prithvi
 */

#ifndef CLOCK_NRF_H_
#define CLOCK_NRF_H_

#include "nrf.h"
#include "clock.h"
#include "leds.h"
#include "nrf_delay.h"
#include "nrf_gpio.h"
#include "simple_uart.h"

#define HFCLK_FREQUENCY		  (16000000UL)
#define LFCLK_FREQUENCY           (32768UL)                               /**< LFCLK frequency in Hertz, constant. */

#define RTC_FREQUENCY             (CLOCK_SECOND)                          /**< Required RTC working clock RTC_FREQUENCY Hertz. Changable. */
#define COUNTER_PRESCALER         ((LFCLK_FREQUENCY/RTC_FREQUENCY) - 1) /* f = LFCLK/(prescaler + 1) */

void rtc_init(void);
void lfclk_init(void);
void lfclk_deinit(void);
void hfclk_xtal_init(void);
void hfclk_xtal_deinit(void);
clock_time_t nrf_clock_time(void);
unsigned long nrf_clock_seconds(void);
void nrf_clock_set_seconds(uint32_t sec);
void etiemr_update_expiration_time(clock_time_t expiration_time);

#endif /* CLOCK_NRF_H_ */
