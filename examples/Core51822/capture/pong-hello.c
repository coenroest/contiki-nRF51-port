

#include "contiki.h"
#include "dev/leds.h"
#include "dev/radio.h"
#include "simple-uart.h"
#include "nrf-radio.h"

#include <stdio.h> /* For printf() */
#include <inttypes.h>

#define DEBUG 1
#if DEBUG
#define PRINTF(...) printf(__VA_ARGS__)
#else
#define PRINTF(...) do {} while (0)
#endif
/*---------------------------------------------------------------------------*/

/* Parameters for sending a ping packet with controls */

#define DEVICE_ID 1

#define SCENARIO 0
#define COUNT 1
#define SENDER 2
#define DELAY 3
#define MULT 4
#define	POWER 5
#define OPTIONAL1 6
#define OPTIONAL2 7

/*---------------------------------------------------------------------------*/
static struct etimer et_blink, et_tx;
static struct rtimer rt;
static uint8_t blinks;
static uint8_t txbuffer[8];  ///< Packet to transmit
static uint8_t rxbuffer[8];  ///< Received packet

rtimer_clock_t rtimer_ref_time, after_blink;
rtimer_clock_t tx_sfd, rx_sfd, delta = 0;
static int tx_delay = 0;

/*---------------------------------------------------------------------------*/
PROCESS(ping_process, "Ping process");
PROCESS(blink_process, "LED blink process");
AUTOSTART_PROCESSES(&ping_process, &blink_process);
/*---------------------------------------------------------------------------*/
static void send(struct rtimer *rt, void *ptr) {

  PRINTF("main-SEND\n\r");

  txbuffer[SCENARIO] = 	rxbuffer[SCENARIO];
  txbuffer[COUNT] = 	rxbuffer[COUNT];
  txbuffer[SENDER] = 	DEVICE_ID;
  txbuffer[DELAY] = 	88;
  txbuffer[MULT] = 	88;
  txbuffer[POWER] = 	88;
  nrf_radio_send (txbuffer, 8);
  PRINTF ("REPLY!\t TX: ----- Packet: %hi %hi %hi %hi %hi %hi\t\t timestamp: %u\n\r",
	  txbuffer[SCENARIO], txbuffer[COUNT], txbuffer[SENDER], txbuffer[DELAY],
	  txbuffer[MULT], txbuffer[POWER], nrf_radio_read_address_timestamp());
  tx_sfd = nrf_radio_read_address_timestamp();

  if (DEVICE_ID == 1)
    {
      PRINTF("<<<<<<< SFD delta: %u\n\r", (tx_sfd - rx_sfd) - (RTIMER_ARCH_SECOND+tx_delay));
    }
  else
    {
      PRINTF("<<<<<<< SFD delta: %u\n\r", (tx_sfd - rx_sfd) - (RTIMER_ARCH_SECOND));
    }

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

      /* do we have a packet pending?
       *
       * TODO CR: create an escape here for the situation of no packet reception
       * Maybe use nrf_radio_receiving_packet() for this?*/
      nrf_radio_pending_packet();
      nrf_radio_read(rxbuffer, 8);
      nrf_radio_off();

      PRINTF ("P0NG\t RX: ----- Last packet: %hi %hi %hi %hi %hi %hi\t\t\n\r",
	      rxbuffer[SCENARIO], rxbuffer[COUNT], rxbuffer[SENDER],
	      rxbuffer[DELAY], rxbuffer[MULT], rxbuffer[POWER]);

      rx_sfd = nrf_radio_read_address_timestamp();

      /* ---- TX ---- */

      if (rxbuffer[SENDER] == 8)	/* Is the packet from the initiator? */
	{
	  if (DEVICE_ID == 1)		/* Node A */
	    {
	      /* Introduce a given delay for the node with higher TX power */
	      tx_delay = rxbuffer[DELAY]*rxbuffer[MULT];

	      /* Schedule a new transmission with that delay */
	      rtimer_set(&rt, nrf_radio_read_address_timestamp()+RTIMER_ARCH_SECOND+tx_delay,1,send,NULL);
	    }
	  if (DEVICE_ID == 2)		/* Node B */
	    {

	      /* Change the TXpower of Node B to the by the initiator requested value */
	      nrf_radio_set_txpower(-1*rxbuffer[POWER]);

	      /* Schedule a new transmission */
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
    etimer_set(&et_blink, CLOCK_SECOND/(2*DEVICE_ID));

    PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_TIMER);

    leds_off(LEDS_ALL);
    leds_on(blinks & LEDS_ALL);
    blinks++;
  }

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
