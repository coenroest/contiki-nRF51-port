

#include "contiki.h"
#include "dev/leds.h"
#include "dev/radio.h"
#include "simple-uart.h"
#include "nrf-radio.h"

#include <stdio.h> /* For printf() */
#include <string.h> /* For memcpy() */
#include <inttypes.h>

#include "capture-scenario.h"

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
#define OPTIONAL2 7
/*---------------------------------------------------------------------------*/
static struct etimer et_blink, et_tx;
static uint8_t blinks;
static uint8_t txbuffer[8];  ///< Packet to transmit
static uint8_t rxbuffer[8];  ///< Received packet

static uint32_t count = 0;
static uint32_t recvA, recvB, recvX, recvX2 = 0;

/*---------------------------------------------------------------------------*/
PROCESS(ping_process, "Ping process");
PROCESS(blink_process, "LED blink process");
AUTOSTART_PROCESSES(&ping_process, &blink_process);
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

  int i = 2;
  int escape = 0;

  while(1)
  {
    etimer_set (&et_tx, 5*CLOCK_SECOND);
    PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_TIMER);

/*    if (count >= 10)
      {
	if (i < sizeof(scenario)/sizeof(scenario[0]))
	  {
	    i++;
	    count = 0;
	  }
	else
	  {
	    i = 0;
	    PRINTF("END OF TESTS\n\r");
	  }
      }*/

    /* Copy the control packet for a specific scenario */
    memcpy(&txbuffer, scenario[0], 8);

    txbuffer[COUNT] = count++;

    /* Adjust it for testing */
    txbuffer[DELAY] = 128;
    txbuffer[MULT] = 1;

    txbuffer[POWERA] = RADIO_TXPOWER_TXPOWER_Pos4dBm;
    txbuffer[POWERB] = RADIO_TXPOWER_TXPOWER_Neg12dBm;





    /* ---- TX ---- */
    nrf_radio_send (txbuffer, 8);
    PRINTF ("PING\t TX: ----- Packet: %u %u %u %u %u %u %u\t\t timestamp: %u\n\r",
    txbuffer[SCENARIO], txbuffer[COUNT], txbuffer[SENDER], txbuffer[DELAY],
    txbuffer[MULT], txbuffer[POWERA], txbuffer[POWERB], nrf_radio_read_address_timestamp());

    /* ---- RX ---- */
    nrf_radio_on();

    /* do we have a packet pending?
    *
    * TODO CR: create an escape here for the situation of no packet reception
    * Maybe use nrf_radio_receiving_packet() for this?*/
    //nrf_radio_pending_packet();

    /* FIXME CR: quick hack to prevent the program from waiting on an
     * END event if the packets have collided.
     */
    while (NRF_RADIO->EVENTS_END == 0 && escape < 400000)
      {
	escape++;
      }
    escape = 0;

    nrf_radio_off();

    /* Read what is in the radio buffer */
    nrf_radio_read(rxbuffer, 8);

    PRINTF ("PING\t RX: ----- Last packet: %hi %hi %hi %hi %hi %hi %hi\t\t\n\r",
    rxbuffer[SCENARIO], rxbuffer[COUNT], rxbuffer[SENDER],
    rxbuffer[DELAY], rxbuffer[MULT], rxbuffer[POWERA], rxbuffer[POWERB]);

/*    PRINTF("RX:\t %hi %hi %hi %hi %hi %hi %hi %hi\n\r",
    rxbuffer[0], rxbuffer[1], rxbuffer[2], rxbuffer[3], rxbuffer[4],
    rxbuffer[5], rxbuffer[6], rxbuffer[7]);*/

    /* Check of whom we received a packet and count it */
    count_score();

    PRINTF("Score --- A: %i - B: %u - X: %u - X2: %u\n\r", recvA, recvB, recvX, recvX2);

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
