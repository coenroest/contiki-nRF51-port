/**
 * \addtogroup nrf51-contiki-rtimer
 * @{
 * \file
 *
 *  Created on: 19-Feb-2014
 *  \author prithvi
 */

#include "rtimer-arch.h"
#include "clock-nrf.h"
#include "sys/rtimer.h"

/** Variable to store the count of the Rtimer */
static volatile rtimer_clock_t rtimer_count = 0;
/** Variable to store the time at which the next Rtimer task needs to be executed */
static volatile rtimer_clock_t rtimer_next_schedule = 0;
/** Boolean value to store if a Rtimer task has been scheduled */
static bool rtimer_scheduled = false;

/** \brief Initialize the RTIMER using TIMER1.
 *			This function makes sure the high frequency clock is running,
 *			initializes TIMER1 based on the parameters defined in
 *			\ref nrf-rtimer-definitions "Macros for RTIMER using nrf51822"
 *			and initializes the compare 0 interrupt.
 */
void rtimer_arch_init(void){
	/* Check if the HF clock is running*/
	if((NRF_CLOCK->HFCLKSTAT & CLOCK_HFCLKSTAT_STATE_Msk) == 0){
		hfclk_xtal_init();
	}
	/* Clear the task to make sure the timer is stopped */
	NRF_TIMER1->TASKS_CLEAR    = 1;
	/* Set the timer in Timer Mode */
	NRF_TIMER1->MODE           = TIMER_MODE_MODE_Timer;
	/* Prescaler 0 produces 16MHz timer tick frequency */
	NRF_TIMER1->PRESCALER      = TIMER_PRESCALER;
	/* 8 bit mode */
	NRF_TIMER1->BITMODE        = TIMER_BITSIZE;
    /* Enable the Compare event on 0th channel */
    NRF_TIMER1->EVENTS_COMPARE[0]  = 0;
    /* The */
    NRF_TIMER1->CC[0]          = 128;

    // Enable overflow event and overflow interrupt:
    NRF_TIMER1->INTENSET      = TIMER_INTENSET_COMPARE0_Msk;

    NVIC_EnableIRQ(TIMER1_IRQn);    // Enable Interrupt for TIMER1 in the core.

    NRF_TIMER1->TASKS_START   = 1;                    		// Start timer.
}

/** Schedule the next Rtimer task
 *
 * \param t time at which next Rtimer task should be executed
 */
void rtimer_arch_schedule(rtimer_clock_t t){
	rtimer_next_schedule = t;
	rtimer_scheduled = true;
}

/**
 * \brief      Get the current clock time
 * \return     This function returns what the real-time module thinks
 *             is the current time. The current time is used to set
 *             the timeouts for real-time tasks.
 *
 */
rtimer_clock_t rtimer_arch_now(void){
	return rtimer_count;
}

/** \brief Function for handling the TIMER1 interrupt.
 * TIMER1 interrupt is triggered on COMPARE[0]. Since the TIMER
 * does not restart upon successful comparison, the TIMER gets an
 * interrupt every 256 ticks when a 8 bit timer is used irrespective
 * of the compare value used. \n In this interrupt routine the rtimer
 * count is incremented. If a rtimer task is scheduled and the rtimer
 * count has reached the next scheduled value then the rtimer's next
 * run function is called.
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
}
/**
 * @}
 */
