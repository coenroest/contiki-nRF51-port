/* This is a very simple hello_world program.
 * It aims to demonstrate the co-existence of two processes:
 * One of them prints a hello world message and the other blinks the LEDs
 *
 * It is largely based on hello_world in $(CONTIKI)/examples/sensinode
 *
 * Author: George Oikonomou - <oikonomou@users.sourceforge.net>
 */

#include "contiki.h"
#include "dev/leds.h"
#include "dev/radio.h"
#include "simple-uart.h"
#include "nrf-radio.h"

#include <stdio.h> /* For printf() */
/*---------------------------------------------------------------------------*/
static struct etimer et_blink, et_rx;
static uint8_t blinks;
static uint8_t rxbuffer[4];  ///< Packet to transmit
/*---------------------------------------------------------------------------*/
PROCESS(rx_process, "RX process");
PROCESS(blink_process, "LED blink process");
AUTOSTART_PROCESSES(&rx_process, &blink_process);
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(rx_process, ev, data)
{
  PROCESS_BEGIN();

  //NRF_RADIO->PACKETPTR = (uint32_t)packet;


  while(1) {

      etimer_set(&et_rx, CLOCK_SECOND);

      PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_TIMER);

      nrf_radio_read(rxbuffer, 4);
      printf("Contents of packet: %d\n\r", (int)*rxbuffer);

  }

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(blink_process, ev, data)
{
  PROCESS_BEGIN();

  blinks = 0;

  while(1) {
    etimer_set(&et_blink, CLOCK_SECOND);

    PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_TIMER);

    leds_off(LEDS_ALL);
    leds_on(blinks & LEDS_ALL);
    blinks++;
    //printf("Blink... (state %c)\n", leds_get());
  }

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
