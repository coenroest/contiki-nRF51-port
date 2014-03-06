/**
 * \addtogroup PCA10000
 * @{
 *
 * \file
 * Configuration for the PCA10000 platform
 *
 *  \author prithvi
 */

#ifndef CONTIKI_CONF_H_
#define CONTIKI_CONF_H_

#include "stdint.h"
#include "stdio.h"
#include "nrf.h"

/*---------------------------------------------------------------------------*/
/**
 * \name Compiler configuration and platform-specific type definitions
 *
 * Clock and RTIMER's frequency is defined here. Also UART baud rate is specified.
 * @{
 */
/* For the main clock. RTC1 is used in nrf51822*/
#define CLOCK_CONF_SECOND 64

/* Compiler configurations */
#define CCIF
#define CLIF

/* Platform typedefs */
typedef uint32_t clock_time_t;
typedef uint32_t uip_stats_t;
typedef uint32_t rtimer_clock_t;

/* rtimer.h typedefs rtimer_clock_t as unsigned short. We need to define
 * RTIMER_CLOCK_LT to override this.
 */
#define RTIMER_CLOCK_LT(a,b)    ((int32_t)((a)-(b)) < 0)
/* Pre-requisite definitions for the RTIMER initialization */
#define TIMER_PRESCALER 	0
#define TIMER_BITSIZE TIMER_BITMODE_BITMODE_08Bit
#if TIMER_BITSIZE == TIMER_BITMODE_BITMODE_08Bit
#define TIMER_COMPARE_FREQ	256
#endif
/* 62.5 kHz clock based on TIMER1 for the RTIMER */
#define RTIMER_ARCH_SECOND 		(((HFCLK_FREQUENCY)/(1<<TIMER_PRESCALER))/TIMER_COMPARE_FREQ)

/* Specify the UART baud rate */
#define UART_BAUDRATE UART_BAUDRATE_BAUDRATE_Baud38400

/* Specify the source of the Low freq clock for the platform */
#define SRC_LFCLK CLOCK_LFCLKSRC_SRC_Xtal


/** @} */
/*---------------------------------------------------------------------------*/

#endif /* CONTIKI_CONF_H_ */
/** @} */
