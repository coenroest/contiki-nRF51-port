/**
 * \addtogroup nrf51822
 * @{
 * \defgroup nrf51-contiki-rtimer Contiki Rtimer implementation for nrf51822 SoC
 * 			This module contains the Rtimer implementation for nrf51822 SoC using
 * 			TIMER1 peripheral running on the High Frequency clock
 *
 * @{
 * \file
 *
 *  Created on: 19-Feb-2014
 * \author prithvi
 */

#ifndef RTIMER_ARCH_H_
#define RTIMER_ARCH_H_

#include "nrf.h"
#include "contiki-conf.h"

void rtimer_arch_init(void);
void rtimer_arch_schedule(rtimer_clock_t t);
rtimer_clock_t rtimer_arch_now(void);

#endif /* RTIMER_ARCH_H_ */
/**
 * @}
 * @}
 */
