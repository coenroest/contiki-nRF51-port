/**
 * \addtogroup Core51822
 * @{
 *
 * \file
 * Platform specific configurations for Core51822.
 * PCA10000's platform specific definitions required for Contiki
 * The parameters defined are Clock and RTIMER's frequency, UART baud rate,
 * Low frequency clock source and typedef related to clock, rtimer and uip stats.
 *  \author prithvi
 */

#ifndef CONTIKI_CONF_H_
#define CONTIKI_CONF_H_


#include "stdint.h"
#include "stdio.h"
#include "stdbool.h"
#include "nrf.h"

//#ifndef NETSTACK_CONF_RADIO
//#define NETSTACK_CONF_RADIO   nrf_radio_driver
//#endif /* NETSTACK_CONF_RADIO */
/*---------------------------------------------------------------------------*/
/** Specify the UART baud rate */
#define UART_BAUDRATE UART_BAUDRATE_BAUDRATE_Baud38400

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

/** \name Platform typedefs are all unsigned 32 bit
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

/** \anchor nrf-rtimer-definitions
 * \name Definitions for the TIMER1 initialization
 * Used in \ref nrf51-contiki-rtimer module
 * @{*/
/** The Rtimer's TIMER tick freq is HF freq divided by two to power of value defined here*/
#define TIMER_PRESCALER 	0
/** \brief Number of bits used for the Rtimer's TIMER */
#define TIMER_BITSIZE TIMER_BITMODE_BITMODE_08Bit
#if TIMER_BITSIZE == TIMER_BITMODE_BITMODE_08Bit
/** Since the Rtimer is 8 bit the CC interrupt occurs every 2^8 ticks of the TIMER */
#define TIMER_COMPARE_FREQ	256
#endif

/** HF Clock frequency in Hertz, constant.
 *  When RC is used it is always 16 MHz. When a crystal is
 *  used it is either 16 MHz or 32 MHz based on the crystal.
 */
#define HFCLK_FREQUENCY		  (16000000UL)

/** \brief 62.5 kHz clock based on TIMER1 for the RTIMER
 *  62.5kHz = 16MHz/((1<<0)x256). To decrease this frequency \ref TIMER_PRESCALER
 *  can be increased till 9 */
#define RTIMER_ARCH_SECOND 		(((HFCLK_FREQUENCY)/(1<<TIMER_PRESCALER))/TIMER_COMPARE_FREQ)
/** @} */

/** \name Definitions for the RTC1 initialization
 * Used in \ref nrf51-contiki-clock module
 * @{*/
/** \brief Frequency of main clock, for which RTC1 is used
 */
#define CLOCK_CONF_SECOND 64
/* Check if CLOCK_CONF_SECOND is power of 2 and less than or equal to 32.768 kHz */
#if (!(!(CLOCK_CONF_SECOND & (CLOCK_CONF_SECOND-1)) && CLOCK_CONF_SECOND && (CLOCK_CONF_SECOND<=32768)))
#error CLOCK_CONF_SECOND must be a power of 2 with a maximum frequency of 32768 Hz
#endif

/** Specify the source of the Low freq clock for the platform */
#define SRC_LFCLK CLOCK_LFCLKSRC_SRC_Xtal

/** Specify if TICKLESS implementation is required*/
#define TICKLESS true
/** @} */
/*---------------------------------------------------------------------------*/

#endif /* CONTIKI_CONF_H_ */
/** @} */
