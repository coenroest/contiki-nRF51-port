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
static struct etimer et_blink, et_tx;
static struct rtimer rt;
static uint8_t blinks;
static uint8_t txbuffer[4];  ///< Packet to transmit
rtimer_clock_t rtimer_ref_time, after_blink;
static int count = 0;

/*---------------------------------------------------------------------------*/
PROCESS(tx_process, "TX process");
PROCESS(blink_process, "LED blink process");
AUTOSTART_PROCESSES(&tx_process, &blink_process);
/*---------------------------------------------------------------------------*/
static void send(struct rtimer *rt, void *ptr) {

  txbuffer[0] = count;
  txbuffer[1] = 42;
  txbuffer[2] = 8;
  txbuffer[3] = 66;
  nrf_radio_send(txbuffer, 8);
  printf ("TX: ----- Packet send: %u %u %u %02x\n\r", txbuffer[0],
  	      txbuffer[1], txbuffer[2], txbuffer[3]);
  count++;
  //nrf_radio_on();

}
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(tx_process, ev, data)
{
  PROCESS_BEGIN();

  while(1)
  {
      etimer_set(&et_tx, 5*CLOCK_SECOND);

      PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_TIMER);


     /* rtimer_ref_time = RTIMER_NOW();
      leds_blink();
      after_blink = RTIMER_NOW();

      printf("--- The time is: %u\n\r", after_blink-rtimer_ref_time);*/

      //printf("----> BEFORE SCHED: %u\n\r", RTIMER_NOW());
      rtimer_set(&rt, RTIMER_NOW()+RTIMER_ARCH_SECOND,1,send,NULL);
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
