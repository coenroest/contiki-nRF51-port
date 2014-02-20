/*
 * clock.c
 *
 *  Created on: 30-Jan-2014
 *      Author: prithvi
 */

#include "clock-nrf.h"
#include "clock.h"
#include "nrf_delay.h"

/**
 * Initialize the clock library.
 *
 * This function initializes the clock library and should be called
 * from the main() function of the system.
 *
 */
void
clock_init(void)
{
  hfclk_xtal_init();
  lfclk_init();
  rtc_init();
}

/**
 * Get the current clock time.
 *
 * This function returns the current system clock time.
 *
 * \return The current clock time, measured in system ticks.
 */
CCIF clock_time_t
clock_time(void)
{
  return nrf_clock_time();
}

/**
 * Get the current value of the platform seconds.
 *
 * This could be the number of seconds since startup, or
 * since a standard epoch.
 *
 * \return The value.
 */
CCIF unsigned long
clock_seconds(void)
{
  return nrf_clock_seconds();
}
/**
 * Set the value of the platform seconds.
 * \param sec   The value to set.
 *
 */
void
clock_set_seconds(unsigned long sec)
{
  nrf_clock_set_seconds(sec);
}

/**
 * Wait for a given number of ticks.
 * \param t   How many ticks.
 *
 */
void
clock_wait(clock_time_t t)
{
  if(t) {
    clock_time_t end_t = t + nrf_clock_time();

    while(nrf_clock_time() != end_t) {
    }
  }
}

/**
 * Delay a given number of microseconds.
 * \param dt   How many milliseconds to delay.
 *
 * \note Interrupts could increase the delay by a variable amount.
 */
void
clock_delay(unsigned int dt)
{
  nrf_delay_ms(dt);
}

/**
 * Delay a given number of microseconds.
 * \param dt   How many microseconds to delay.
 *
 * \note Interrupts could increase the delay by a variable amount.
 */
void
clock_delay_usec(uint16_t dt)
{
  nrf_delay_us(dt);
}
