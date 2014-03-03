/*
 * contiki-conf.h
 *
 *  Created on: 30-Jan-2014
 *      Author: prithvi
 */

#ifndef CONTIKI_CONF_H_
#define CONTIKI_CONF_H_

#include <stdint.h>
#include "nrf.h"

#define CCIF
#define CLIF

/* For the main clock. RTC1 is used in nrf51822*/
#define CLOCK_CONF_SECOND 64
typedef uint32_t clock_time_t;

/* For the RTIMER clock. Timer1 is used in nrf51822*/
#define TIMER_PRESCALER 	0
#define TIMER_BITSIZE TIMER_BITMODE_BITMODE_08Bit
#if TIMER_BITSIZE == TIMER_BITMODE_BITMODE_08Bit
#define TIMER_COMPARE_FREQ	256
#endif
#define RTIMER_ARCH_SECOND (((HFCLK_FREQUENCY)/(1<<TIMER_PRESCALER))/TIMER_COMPARE_FREQ)
#define RTIMER_CLOCK_LT(a,b) ((int32_t)((a)-(b)) < 0)
typedef uint32_t rtimer_clock_t;

/* Defined as 0 for UART0 and 1 for UART1 */
//#define DEBUG_UART      0

typedef uint32_t uip_stats_t;

#endif /* CONTIKI_CONF_H_ */
