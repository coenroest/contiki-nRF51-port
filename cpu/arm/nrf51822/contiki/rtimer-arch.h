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
#include "contiki-conf.h"

void rtimer_arch_init(void);
void rtimer_arch_schedule(rtimer_clock_t t);
rtimer_clock_t rtimer_arch_now(void);

#endif /* RTIMER_ARCH_H_ */
