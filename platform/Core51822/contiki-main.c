/**
 * \addtogroup platform
 * @{
 *
 * \defgroup Core51822 Core51822: The Waveshare platform for the nrf51822 MCU
 *
 * @{
 * \file
 * Main file where the code execution starts for all examples based on Core51822 platform
 *  \author CoenRoest
 */

#include "contiki.h"
#include "board.h"
#include "simple-uart.h"
#include "dev/leds.h"

#include "nrf-radio.h"
#include "nrf-init.h"

/*---------------------------------------------------------------------------*/
/**
 * \brief Main routine for nrf51822's Core51822 platform
 */
int
main(void)
{
/*
 * Initialize the basic peripherals required for Contiki
 */
  clock_init();
  rtimer_init();
  leds_init();

  process_init();
  simple_uart_init();

  /* Testing of the basic peripherals */
  leds_blink();
  printf("Testing\n\r");

  process_start(&etimer_process, NULL);
  ctimer_init();

  /* Configure the nRF51 inits */
  nrf_init();

  /* Configure the nRF51 radio */
  nrf_radio_init();

  autostart_start(autostart_processes);

  while(true) {
    uint32_t process_count;
    do {
    	process_count = process_run();
    } while(process_count > 0);

    /* We have serviced all pending events. Enter a Low-Power mode. */
    __WFE();
  }
}
/*---------------------------------------------------------------------------*/

/**
 * @}
 * @}
 */
