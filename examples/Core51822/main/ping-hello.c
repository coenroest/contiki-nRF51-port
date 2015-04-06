

#include "contiki.h"
#include "dev/leds.h"
#include "dev/radio.h"
#include "simple-uart.h"
#include "nrf-radio.h"
#include "dev/leds.h"
#include "dev/radio.h"
#include "net/packetbuf.h"
#include "net/netstack.h"
#include "random.h"


#include <stdio.h> /* For printf() */
#include <string.h> /* For memcpy() */
#include <inttypes.h>

#define DEBUG 1
#if DEBUG
#define PRINTF(...) printf(__VA_ARGS__)
#else
#define PRINTF(...) do {} while (0)
#endif
/*---------------------------------------------------------------------------*/

/* Parameters for sending a ping packet with controls */

#define DEVICE_ID 8

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
static uint8_t blinks;
static uint8_t txbuffer[28];  ///< Packet to transmit
static uint8_t rxbuffer[28];  ///< Received packet

static uint32_t count = 0;
static uint32_t recvA, recvB, recvX, recvX2 = 0;

/* The escape counter for escaping the waiting while loop for the end of a packet */
int escape = 0;

/* Vars to find the real SNR */
int signal, noise, ratio = 0;

/*---------------------------------------------------------------------------*/
PROCESS(ping_process, "Ping process");
PROCESS(blink_process, "LED blink process");
AUTOSTART_PROCESSES(&ping_process);//, &blink_process);
/*---------------------------------------------------------------------------*/
static void count_score(void)
{
  /* Check what the content of the received packet is and increase
   * the scores of the nodes
   */
  if (rxbuffer[SENDER] == 1)
  	{
  	  recvA++;
  	}
        else if (rxbuffer[SENDER] == 2)
  	{
  	  recvB++;
  	}
        else if (rxbuffer[SENDER] == 6)
  	{
  	  recvX++;
  	}
        else
  	{
  	  recvX2++;
  	}
}
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(ping_process, ev, data)
{
  PROCESS_BEGIN();

  while(1)
    {
      //etimer_set (&et_tx, CLOCK_SECOND * 4 + random_rand() % (CLOCK_SECOND * 4));
      etimer_set (&et_tx, CLOCK_SECOND * 5);// + random_rand() % (CLOCK_SECOND * 1));

      PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_TIMER);

    txbuffer[SCENARIO] 	= 	55;
    txbuffer[COUNT] 	= 	count++;
    txbuffer[SENDER] 	=	DEVICE_ID;
    txbuffer[23]	=	88;
    txbuffer[24]	=	77;


    /* ---- TX ---- */

    // Change this to not hardcode the packet length
    nrf_radio_send (txbuffer, 25);

    PRINTF ("PING\t TX: ----- Packet: %hi %hi %hi %hi %hi %hi %hi %hi\n\r",
    txbuffer[SCENARIO], txbuffer[COUNT], txbuffer[SENDER], txbuffer[DELAY],
    txbuffer[MULT], txbuffer[POWERA], txbuffer[POWERB], txbuffer[OPTIONAL]);

    /* ---- RX ---- */
    //nrf_radio_on();
    NETSTACK_RADIO.on();

    /* do we have a packet pending?*/

    /* FIXME CR: quick hack to prevent the program from waiting on an
     * Radio END Event if the packets have collided.
     */
    while (NRF_RADIO->EVENTS_END == 0 && escape < 400000)
      {
	escape++;
      }
    /* Reset the escape counter */
    escape = 0;

    /* Switch the radio off */
    //nrf_radio_off();
    NETSTACK_RADIO.off();

    /* Read what is in the radio buffer */
    nrf_radio_read(rxbuffer, sizeof(rxbuffer));

    PRINTF ("PING\t RX: ----- Last packet: %hi %hi %hi %hi %hi %hi %hi %hi\t\tRSSI: %i\n\r",
    rxbuffer[SCENARIO], rxbuffer[COUNT], rxbuffer[SENDER], rxbuffer[DELAY],
    rxbuffer[MULT], rxbuffer[POWERA], rxbuffer[POWERB], rxbuffer[OPTIONAL], nrf_radio_rssi());

    /* Check of whom we received a packet and count it */

    count_score();

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
