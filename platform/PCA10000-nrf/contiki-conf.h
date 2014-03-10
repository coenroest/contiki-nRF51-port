/**
 * \addtogroup PCA10000
 * @{
 *
 * \file
 * Platform specific configurations for PCA10000
 * PCA10000's platform specific definitions required for Contiki
 * The parameters defined are Clock and RTIMER's frequency, UART baud rate,
 * Low frequency clock source and typedef related to clock, rtimer and uip stats.
 *  \author prithvi
 */

#ifndef CONTIKI_CONF_H_
#define CONTIKI_CONF_H_


#include "stdint.h"
#include "stdio.h"
#include "nrf.h"

/*---------------------------------------------------------------------------*/

/** Frequency of main clock, for which RTC1 is used*/
#define CLOCK_CONF_SECOND 64

/** \name Compiler configurations.
 * CCIF and CLIF are defined but used only in Windows based platforms
 * \code#define CCIF
 *  #define CLIF \endcode
 * @{
 */
/** \showinitializer */
#define CCIF
/** \showinitializer */
#define CLIF
/** @} */

/** \name Platform typedefs
 * @{ */
typedef uint32_t clock_time_t;
typedef uint32_t uip_stats_t;
typedef uint32_t rtimer_clock_t;
/** @} */

/** TRUE if a is less than b, otherwise FALSE.
 * rtimer.h typedefs rtimer_clock_t as unsigned short.
 * We need to define RTIMER_CLOCK_LT here to override this.
 */
#define RTIMER_CLOCK_LT(a,b)    ((int32_t)((a)-(b)) < 0)

/** \name Pre-requisite definitions for the RTIMER initialization
 * @{*/
#define TIMER_PRESCALER 	0
#define TIMER_BITSIZE TIMER_BITMODE_BITMODE_08Bit
#if TIMER_BITSIZE == TIMER_BITMODE_BITMODE_08Bit
#define TIMER_COMPARE_FREQ	256
#endif
/** @} */

/** HF Clock frequency in Hertz, constant.
 *  When RC is used it is always 16 MHz. When a crystal is
 *  used it is either 16 MHz or 32 MHz based on the crystal.
 */
#define HFCLK_FREQUENCY		  (16000000UL)

/** 62.5 kHz clock based on TIMER1 for the RTIMER */
#define RTIMER_ARCH_SECOND 		(((HFCLK_FREQUENCY)/(1<<TIMER_PRESCALER))/TIMER_COMPARE_FREQ)

/** Specify the UART baud rate */
#define UART_BAUDRATE UART_BAUDRATE_BAUDRATE_Baud38400

/** Specify the source of the Low freq clock for the platform */
#define SRC_LFCLK CLOCK_LFCLKSRC_SRC_Xtal

/** Specify if TICKLESS implementation is required*/
#define TICKLESS TRUE

/*---------------------------------------------------------------------------*/

#endif /* CONTIKI_CONF_H_ */
/** @} */
