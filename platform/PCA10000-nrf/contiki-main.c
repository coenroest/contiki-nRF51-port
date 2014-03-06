/**
 * \addtogroup platform
 * @{
 *
 * \defgroup PCA10000 PCA10000: The USB dongle platform for the nrf51822 MCU
 *
 * PCA10000 is a USB dongle as a part of the nrf51822 MCU's evaluation and development kit.
 * nrf51822 is a ARM Cortex M0 based MCU with BLE compatible radio. It has an inbuilt JLink-Lite
 * Cortex-M debugger/programmer.
 * @{
 * \file contiki-main.c
 *
 *  Created on: 30-Feb-2014
 *  \author prithvi
 */

#include "contiki.h"
#include "board.h"
#include "simple_uart.h"

/*---------------------------------------------------------------------------*/
/**
 * \brief Main routine for nrf51822's PCA10000 platform
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
  simple_uart_init();

  process_init();

  /* Testing of the basic peripherals */
  leds_blink();
  printf("Testing\r");

  process_start(&etimer_process, NULL);
  ctimer_init();

  autostart_start(autostart_processes);

  while(true) {
    uint32_t process_count;
    do {
    	process_count = process_run();
    } while(process_count > 0);

    /* We have serviced all pending events. Enter a Low-Power mode. */
    __WFI();
  }
}
/*---------------------------------------------------------------------------*/

/**
 * @}
 * @}
 */
