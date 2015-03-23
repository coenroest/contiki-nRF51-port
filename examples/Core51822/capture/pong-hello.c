

#include "contiki.h"
#include "dev/leds.h"
#include "dev/radio.h"
#include "simple-uart.h"
#include "nrf-radio.h"

#include <stdio.h> /* For printf() */
#include <inttypes.h>
/*---------------------------------------------------------------------------*/

/* Parameters for sending a ping packet with controls */

#define DEVICE_ID 2

#define COUNT 0
#define SENDER 1
#define DELAY 2
#define MULT 3
#define	POWER 4

/*---------------------------------------------------------------------------*/
static struct etimer et_blink, et_tx;
static struct rtimer rt;
static uint8_t blinks;
static uint8_t txbuffer[5];  ///< Packet to transmit
static uint8_t rxbuffer[5];  ///< Received packet

rtimer_clock_t rtimer_ref_time, after_blink;
static int delay = 0;

/*---------------------------------------------------------------------------*/
PROCESS(ping_process, "Ping process");
PROCESS(blink_process, "LED blink process");
AUTOSTART_PROCESSES(&ping_process, &blink_process);
/*---------------------------------------------------------------------------*/
static void send(struct rtimer *rt, void *ptr) {

  printf("main-SEND\n\r");

  txbuffer[COUNT] = 88;
  txbuffer[SENDER] = DEVICE_ID;
  txbuffer[DELAY] = 0;
  txbuffer[MULT] = 88;
  txbuffer[POWER] = 88;
  nrf_radio_send (txbuffer, 5);
  printf ("REPLY!\t TX: ----- Packet: %u %u %u %u %u\t\t timestamp: %u\n\r", txbuffer[COUNT],
      	      txbuffer[SENDER], txbuffer[DELAY], txbuffer[MULT], txbuffer[POWER], NRF_TIMER0->CC[TIMESTAMP_REG]);

}
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(ping_process, ev, data)
{
  PROCESS_BEGIN();

  while(1)
  {
      etimer_set (&et_tx, 5*CLOCK_SECOND);
      PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_TIMER);


      /* ---- RX ---- */
      nrf_radio_on();
      /* do we have a packet pending? */
      nrf_radio_pending_packet();
      nrf_radio_read(rxbuffer, 5);
      nrf_radio_off();


      printf ("P0NG\t RX: ----- Last packet: %u %u %u %u %u\t\t\n\r",
      	      rxbuffer[COUNT], rxbuffer[SENDER], rxbuffer[DELAY], rxbuffer[MULT], rxbuffer[POWER]);


      if (rxbuffer[SENDER] == 8)
	{
	  if (DEVICE_ID == 1)
	    {
	      delay = rxbuffer[DELAY]*rxbuffer[MULT];
	      NRF_RADIO->TXPOWER = (RADIO_TXPOWER_TXPOWER_Pos4dBm << RADIO_TXPOWER_TXPOWER_Pos);
	      rtimer_set(&rt, nrf_radio_read_address_timestamp()+RTIMER_ARCH_SECOND+delay,1,send,NULL);
	    }
	  if (DEVICE_ID == 2)
	    {
	      NRF_RADIO->TXPOWER = (RADIO_TXPOWER_TXPOWER_Neg30dBm << RADIO_TXPOWER_TXPOWER_Pos);
	      rtimer_set(&rt, nrf_radio_read_address_timestamp()+RTIMER_ARCH_SECOND,1,send,NULL);
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
    etimer_set(&et_blink, CLOCK_SECOND/4);

    PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_TIMER);

    leds_off(LEDS_ALL);
    leds_on(blinks & LEDS_ALL);
    blinks++;
  }

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
