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
static struct etimer et_blink;
static uint8_t blinks;
static uint8_t packet[4];  ///< Packet to transmit

/*---------------------------------------------------------------------------*/
PROCESS(tx_process, "TX process");
PROCESS(blink_process, "LED blink process");
AUTOSTART_PROCESSES(&tx_process, &blink_process);
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(tx_process, ev, data)
{
  PROCESS_BEGIN();

  // Set payload pointer
    NRF_RADIO->PACKETPTR = (uint32_t)packet;

  while(1) {

      nrf_radio_transmit(4);

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
