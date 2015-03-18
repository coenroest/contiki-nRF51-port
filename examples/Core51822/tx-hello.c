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
static uint8_t txbuffer[32];  ///< Packet to transmit
rtimer_clock_t rtimer_ref_time, after_blink;
static int count = 0;

/*---------------------------------------------------------------------------*/
PROCESS(tx_process, "TX process");
PROCESS(blink_process, "LED blink process");
AUTOSTART_PROCESSES(&tx_process, &blink_process);
/*---------------------------------------------------------------------------*/
static void send(struct rtimer *rt, void *ptr) {

  printf("----> AFTER SCHED: %u\n\r", RTIMER_NOW());
  NRF_RADIO->INTENSET = RADIO_INTENSET_BCMATCH_Msk;

  txbuffer[0] = count;
  nrf_radio_send(txbuffer, 4);
  printf("Contents of packet: %d\t\t address timestamp: %u\n\r", (int)*txbuffer, NRF_TIMER0->CC[TIMESTAMP_REG]);
  count++;


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


      printf("----> BEFORE SCHED: %u\n\r", RTIMER_NOW());
      rtimer_set(&rt, RTIMER_NOW()+RTIMER_ARCH_SECOND,1,send,NULL);
      //rtimer_set(&rt, RTIMER_NOW()+(2*RTIMER_ARCH_SECOND),1,send,NULL);


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
