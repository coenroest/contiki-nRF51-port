

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

#define FIXED_DELAY RTIMER_ARCH_SECOND / 10

#define SCENARIO 0
#define COUNT 1
#define SENDER 2
#define DELAY 3
#define MULT 4
#define	POWERA 5
#define POWERB 6
#define OPTIONAL 7

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
AUTOSTART_PROCESSES(&ping_process);//, &blink_process);
/*---------------------------------------------------------------------------*/
static void send(struct rtimer *rt, void *ptr) {

  nrf_radio_transmit(8);

  PRINTF ("REPLY %u!\t TX: ----- Packet: %hi %hi %hi %hi %hi %hi %hi %hi\n\r",
	  DEVICE_ID, txbuffer[SCENARIO], txbuffer[COUNT], txbuffer[SENDER], txbuffer[DELAY],
	  txbuffer[MULT], txbuffer[POWERA], txbuffer[POWERB], txbuffer[OPTIONAL]);
  tx_sfd = nrf_radio_read_address_timestamp();


  /* Find out if the packet has a large processing time */
  if (DEVICE_ID == 1)
    {
      PRINTF("<<<<<<< SFD delta: %u\n\r", (tx_sfd - rx_sfd) - (FIXED_DELAY+tx_delay));
    }
  else
    {
      PRINTF("<<<<<<< SFD delta: %u\n\r", (tx_sfd - rx_sfd) - (FIXED_DELAY));
    }

}
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(ping_process, ev, data)
{
  PROCESS_BEGIN();

  while(1)
  {
      etimer_set (&et_tx, CLOCK_SECOND);
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

      PRINTF ("P0NG %u\t RX: ----- Last packet: %hi %hi %hi %hi %hi %hi %hi %hi\t\t\n\r",
	      DEVICE_ID, rxbuffer[SCENARIO], rxbuffer[COUNT], rxbuffer[SENDER],
	      rxbuffer[DELAY], rxbuffer[MULT], rxbuffer[POWERA], rxbuffer[POWERB], rxbuffer[OPTIONAL]);

      rx_sfd = nrf_radio_read_address_timestamp();

      /* ---- TX ---- */

      if (rxbuffer[SENDER] == 8)	/* Is the packet from the initiator? */
	{
	  if (DEVICE_ID == 1 && rxbuffer[OPTIONAL] != 2)		/* Node A */
	    {
	      /* Introduce a given delay for the node with higher TX power */
	      tx_delay = rxbuffer[DELAY]*rxbuffer[MULT];

	      /* Change the TXpower of Node A to the by the initiator requested value */
	      nrf_radio_set_txpower(rxbuffer[POWERA]);

	      /* Schedule a new transmission with that delay */
	      rtimer_set(&rt, nrf_radio_read_address_timestamp()+FIXED_DELAY+tx_delay,1,send,NULL);
	    }
	  if (DEVICE_ID == 2 && rxbuffer[OPTIONAL] != 1)		/* Node B */
	    {

	      /* Change the TXpower of Node B to the by the initiator requested value */
	      nrf_radio_set_txpower(rxbuffer[POWERB]);

	      /* Schedule a new transmission */
	      rtimer_set(&rt, nrf_radio_read_address_timestamp()+FIXED_DELAY,1,send,NULL);
	    }

	  /* Prepare the packet */

	  txbuffer[SCENARIO] = 	rxbuffer[SCENARIO];
	  txbuffer[COUNT] = 	rxbuffer[COUNT];
	  txbuffer[SENDER] = 	DEVICE_ID;
	  txbuffer[DELAY] = 	12;
	  txbuffer[MULT] = 	34;
	  txbuffer[POWERA] = 	56;
	  txbuffer[POWERB] = 	78;
	  txbuffer[OPTIONAL] = 	rxbuffer[OPTIONAL];

	  nrf_radio_prepare(txbuffer, 8);

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
