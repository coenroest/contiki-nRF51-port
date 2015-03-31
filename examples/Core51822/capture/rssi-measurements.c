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
#include "nrf-delay.h"

#include <stdio.h> /* For printf() */
/*---------------------------------------------------------------------------*/
static struct etimer et_blink, et_tx;
static uint8_t blinks;

uint32_t freq = 0;
/*---------------------------------------------------------------------------*/
PROCESS(rssi_process, "RSSI process");
PROCESS(blink_process, "LED blink process");
AUTOSTART_PROCESSES(&rssi_process, &blink_process);
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(rssi_process, ev, data)
{
  PROCESS_BEGIN();


  while(1)
  {
      etimer_set(&et_tx, 0.5*CLOCK_SECOND);

      PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_TIMER);

      NRF_RADIO->FREQUENCY = freq;
      NRF_RADIO->EVENTS_READY = 0U;
      NRF_RADIO->TASKS_RXEN = 1U;
      while (NRF_RADIO->EVENTS_READY == 0U);
      NRF_RADIO->EVENTS_END = 0U;

      NRF_RADIO->TASKS_START = 1U;

      NRF_RADIO->EVENTS_RSSIEND = 0U;
      NRF_RADIO->TASKS_RSSISTART = 1U;
/*      nrf_delay_us(40);
      NRF_RADIO->TASKS_RSSISTOP = 1U;*/
      while(NRF_RADIO->EVENTS_RSSIEND == 0U);

      //rssi[i] = NRF_RADIO->RSSISAMPLE;
      printf ("RSSI channel %u: %u\n\r", freq, NRF_RADIO->RSSISAMPLE);

      NRF_RADIO->EVENTS_DISABLED = 0U;
      NRF_RADIO->TASKS_DISABLE = 1U;  // Disable the radio.

      while (NRF_RADIO->EVENTS_DISABLED == 0U)
	{
	  // Do nothing.
	}

      if (freq < 100)
	{
	  freq++;
	}
      else
	return 0;


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

    /*leds_off(LEDS_ALL);
    leds_on(blinks & LEDS_ALL);
    blinks++;*/
  }

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
