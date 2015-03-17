#ifndef NRF_TIMER_H_
#define NRF_TIMER_H_

#include "nrf.h"
#include "contiki-conf.h"

#define TIMER0_IRQ_PRI			5

void nrf_timer_init(void);
void nrf_timer_clear(void);


#endif /* NRF_TIMER_H_ */
