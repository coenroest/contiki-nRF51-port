/*
 * rtimer-arch.h
 *
 *  Created on: 19-Feb-2014
 *      Author: prithvi
 */

#ifndef RTIMER_ARCH_H_
#define RTIMER_ARCH_H_

#include "nrf.h"
#include "clock-nrf.h"

/* For the RTIMER clock. Timer1 is used in nrf51822*/
#define TIMER_PRESCALER 	0
#define TIMER_BITSIZE TIMER_BITMODE_BITMODE_08Bit
#if TIMER_BITSIZE == TIMER_BITMODE_BITMODE_08Bit
#define TIMER_COMPARE_FREQ	256
#endif
#define RTIMER_ARCH_SECOND (((HFCLK_FREQUENCY)/(1<<TIMER_PRESCALER))/TIMER_COMPARE_FREQ)
#define RTIMER_CLOCK_LT(a,b) ((int32_t)((a)-(b)) < 0)

typedef uint32_t rtimer_clock_t;

void rtimer_arch_init(void);
void rtimer_arch_schedule(rtimer_clock_t t);
rtimer_clock_t rtimer_arch_now(void);

#endif /* RTIMER_ARCH_H_ */
