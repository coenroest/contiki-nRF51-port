/*
 * rtimer-arch.c
 *
 *  Created on: 19-Feb-2014
 *      Author: prithvi
 */

#include "rtimer-arch.h"
#include "clock-nrf.h"
#include "sys/rtimer.h"

static volatile rtimer_clock_t rtimer_count = 0, rtimer_next_schedule = 0;
bool rtimer_scheduled = false;

/* Initialize the RTIMER using TIMER1 */
void rtimer_arch_init(void){
	/* Check if the HF clock is running*/
	if((NRF_CLOCK->HFCLKSTAT & CLOCK_HFCLKSTAT_STATE_Msk) == 0){
		hfclk_xtal_init();
	}

	NRF_TIMER1->MODE           = TIMER_MODE_MODE_Timer;        // Set the timer in Timer Mode.
	NRF_TIMER1->PRESCALER      = TIMER_PRESCALER;				// Prescaler 9 produces 31250 Hz timer frequency => 1 tick = 32 us.
	NRF_TIMER1->BITMODE        = TIMER_BITSIZE;  				// 8 bit mode.
	NRF_TIMER1->TASKS_CLEAR    = 1;                            // clear the task first to be usable for later.
    NRF_TIMER1->EVENTS_COMPARE[0]  = 0;

    NRF_TIMER1->CC[0]          = 128;

    // Enable overflow event and overflow interrupt:
    NRF_TIMER1->INTENSET      = TIMER_INTENSET_COMPARE0_Msk;

    NVIC_EnableIRQ(TIMER1_IRQn);    // Enable Interrupt for TIMER1 in the core.

    NRF_TIMER1->TASKS_START   = 1;                    		// Start timer.
}

void rtimer_arch_schedule(rtimer_clock_t t){
	rtimer_next_schedule = t;
	rtimer_scheduled = true;
}

/**
 * \brief      Get the current clock time
 * \return     The current time
 *
 *             This function returns what the real-time module thinks
 *             is the current time. The current time is used to set
 *             the timeouts for real-time tasks.
 *
 */
rtimer_clock_t rtimer_arch_now(void){
	return rtimer_count;
}

/** @brief: Function for handling the TIMER1 interrupts.h
 * Triggered on COMPARE[0]
 */
void
TIMER1_IRQHandler(){
    NRF_TIMER1->EVENTS_COMPARE[0]  = 0;
    rtimer_count++;
    if(rtimer_scheduled){
    	if(rtimer_count >= rtimer_next_schedule){
    		rtimer_scheduled = false;
    	    rtimer_run_next();
    	}
    }
    //if((rtimer_count % (RTIMER_ARCH_SECOND-340)) == 0){
    //	printf("RTIMER count:%d\n",(int) rtimer_count);
    //}
}
