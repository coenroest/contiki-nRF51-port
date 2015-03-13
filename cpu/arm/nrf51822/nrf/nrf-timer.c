
#include "nrf-timer.h"
#include "clock-nrf.h"
#include "sys/rtimer.h"

void nrf_timer_init(void){

  /* Check if the HF clock is running*/
  if((NRF_CLOCK->HFCLKSTAT & CLOCK_HFCLKSTAT_STATE_Msk) == 0){
    hfclk_xtal_init();
  }

  /* Clear the timer before initializing it */
  NRF_TIMER0->TASKS_CLEAR = 1;

  /* Set TIMER0 in Timer mode */
  NRF_TIMER0->MODE      = TIMER_MODE_MODE_Timer;

  /* Set the BIT mode to 32 bit */
  NRF_TIMER0->BITMODE   = TIMER_BITMODE_BITMODE_32Bit;

  /* Set the prescaler to 0 -> 16MHz frequency */
  NRF_TIMER0->PRESCALER = 0;

  /* Start the timer */
  NRF_TIMER0->TASKS_START = 1;

}

void nrf_timer_clear(void){

  /* Clear the timer! */
    NRF_TIMER0->TASKS_CLEAR = 1;
}
