/*
 * main.c
 *
 *  Created on: 30-Feb-2014
 *      Author: prithvi
 */

#include "contiki.h"
#include "board.h"
#include "dev/serial-line.h"


#include <stdint.h>
#include <string.h>
#include <stdio.h>

/*---------------------------------------------------------------------------*/
/**
 * \brief Main routine for nrf51822's PCA10000 platform
 */
int
main(void)
{
  clock_init();
  rtimer_init();
  leds_init();
  simple_uart_config(RTS_PIN_NUMBER, TX_PIN_NUMBER, CTS_PIN_NUMBER,
		  RX_PIN_NUMBER, UART_BAUDRATE_BAUDRATE_Baud38400, 12, HWFC);

  process_init();

  serial_line_init();

  leds_blink();
  printf(" Net: ");

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
