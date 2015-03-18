

#include "contiki.h"
#include "dev/leds.h"
#include "dev/radio.h"
#include "simple-uart.h"
#include "nrf-radio.h"

#include <stdio.h> /* For printf() */
/*---------------------------------------------------------------------------*/

/* Parameters for sending a ping packet with controls */

#define DEVICE_ID 0

#define DELAY_TICKS 0

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
static int count = 0;

/*---------------------------------------------------------------------------*/
PROCESS(ping_process, "Ping process");
PROCESS(blink_process, "LED blink process");
AUTOSTART_PROCESSES(&ping_process, &blink_process);
/*---------------------------------------------------------------------------*/
static void send(struct rtimer *rt, void *ptr) {

  printf("----> AFTER SCHED: %u\n\r", RTIMER_NOW());
}
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(ping_process, ev, data)
{
  PROCESS_BEGIN();

  while(1)
  {
      etimer_set (&et_tx, 5*CLOCK_SECOND);
      PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_TIMER);

      if (count < 1)
	{
	  txbuffer[COUNT] = count;
	  txbuffer[SENDER] = DEVICE_ID;
	  txbuffer[DELAY] = DELAY_TICKS;
	  txbuffer[OPTIONAL] = 0x42;

	  nrf_radio_send (txbuffer, 4);
	  printf ("PING\t TX: ----- Packet send: %u\t%u\t%u\t%02x\n\r", txbuffer[0], txbuffer[1],
		  txbuffer[2], txbuffer[3]);
	  printf ("PING\t TX: ----- Address timestamp: %u\n\r", NRF_TIMER0->CC[TIMESTAMP_REG]);
	  count++;
	}

      nrf_radio_read (rxbuffer, 4);
      printf ("PING\t RX: ----- Content of rxbuffer: %d\t%d\t%d\t%02x\n\r", rxbuffer[0], rxbuffer[1],
      		  rxbuffer[2], rxbuffer[3]);

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
  }

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
