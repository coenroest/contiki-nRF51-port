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

/* Parameters for sending a ping packet with controls */

#define DEVICE_ID 1

/* The fixed delay used for scheduling with the RTimer */
#define DELAY_FIXED 1000

#define COUNT 0
#define SENDER 1
#define DELAY 2
#define OPTIONAL 3

/*---------------------------------------------------------------------------*/
static struct etimer et_blink, et_tx;
static struct rtimer rt;
static uint8_t blinks;
static uint8_t txbuffer[32];  ///< Packet to transmit
static uint8_t rxbuffer[32];  ///< Received packet
rtimer_clock_t rtimer_ref_time, after_blink;
static int count = 100;
int delay = 0;
/*---------------------------------------------------------------------------*/
PROCESS(pong_process, "Pong process");
PROCESS(blink_process, "LED blink process");
AUTOSTART_PROCESSES(&pong_process, &blink_process);
/*---------------------------------------------------------------------------*/
static void send(struct rtimer *rt, void *ptr) {

  nrf_radio_send(txbuffer, 4);

  printf ("P0NG\t TX: ----- Packet send: %u\t%u\t%u\t%02x\n\r", txbuffer[0], txbuffer[1],
	  txbuffer[2], txbuffer[3]);
  printf ("P0NG\t TX: ----- Address timestamp: %u\n\r", nrf_radio_read_address_timestamp());
  count++;

}
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(pong_process, ev, data)
{
  PROCESS_BEGIN();

  while(1)
  {
      etimer_set(&et_tx, CLOCK_SECOND);

      PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_TIMER);

      nrf_radio_read(rxbuffer, 4);
      printf ("P0NG\t RX: ----- Content of rxbuffer: %d\t%d\t%d\t%02x\n\r", rxbuffer[0], rxbuffer[1],
            		  rxbuffer[2], rxbuffer[3]);

      if (rxbuffer[SENDER] == 0)
	{
	  /* Read the desired delay value from receive buffer */
	  delay = rxbuffer[DELAY];

	  /* Make a pong packet */
	  txbuffer[COUNT] = count;
	  txbuffer[SENDER] = DEVICE_ID;
	  txbuffer[OPTIONAL] = 0x24;

	  if (DEVICE_ID == 1)
	    {
	      txbuffer[DELAY] = delay;
	      rtimer_set(&rt, nrf_radio_read_address_timestamp()+DELAY_FIXED+delay,1,send,NULL);
	    }
	  else
	    {
	      txbuffer[delay] = 0;
	      rtimer_set(&rt, nrf_radio_read_address_timestamp()+DELAY_FIXED,1,send,NULL);
	    }
	}
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
