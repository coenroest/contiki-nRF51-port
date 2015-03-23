

#include "contiki.h"
#include "dev/leds.h"
#include "dev/radio.h"
#include "simple-uart.h"
#include "nrf-radio.h"

#include <stdio.h> /* For printf() */
#include <inttypes.h>
/*---------------------------------------------------------------------------*/

/* Parameters for sending a ping packet with controls */

#define DEVICE_ID 8

#define DELAY_TICKS 250
#define MULTIPLIER  11

#define COUNT 0
#define SENDER 1
#define DELAY 2
#define OPTIONAL 3

/*---------------------------------------------------------------------------*/
static struct etimer et_blink, et_tx;
static struct rtimer rt;
static uint8_t blinks;
static uint8_t txbuffer[4];  ///< Packet to transmit
static uint8_t rxbuffer[4];  ///< Received packet

rtimer_clock_t rtimer_ref_time, after_blink;
static uint32_t count = 0;
static uint32_t recvA = 0;
static uint32_t recvB = 0;
static uint32_t recvX = 0;

/*---------------------------------------------------------------------------*/
PROCESS(ping_process, "Ping process");
PROCESS(blink_process, "LED blink process");
AUTOSTART_PROCESSES(&ping_process, &blink_process);
/*---------------------------------------------------------------------------*/
static void send(struct rtimer *rt, void *ptr) {

  printf("main-SEND\n\r");

  txbuffer[COUNT] = 88;
  txbuffer[SENDER] = DEVICE_ID;
  txbuffer[DELAY] = DELAY_TICKS;
  txbuffer[OPTIONAL] = 0x24;
  nrf_radio_send (txbuffer, 8);
  printf ("PING\t TX: ----- Packet: %u %u %u %02x\t\t timestamp: %u\n\r", txbuffer[0],
	      txbuffer[1], txbuffer[2], txbuffer[3], NRF_TIMER0->CC[TIMESTAMP_REG]);


}
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(ping_process, ev, data)
{
  PROCESS_BEGIN();

  while(1)
  {
      etimer_set (&et_tx, 5*CLOCK_SECOND);
      PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_TIMER);

      txbuffer[COUNT] = count++;
      txbuffer[SENDER] = DEVICE_ID;
      txbuffer[DELAY] = DELAY_TICKS;
      txbuffer[OPTIONAL] = MULTIPLIER;
      nrf_radio_send (txbuffer, 4);
      printf ("PING\t TX: ----- Packet: %u %u %u %02x\t\t timestamp: %u\n\r", txbuffer[COUNT],
    	      txbuffer[SENDER], txbuffer[DELAY], txbuffer[OPTIONAL], NRF_TIMER0->CC[TIMESTAMP_REG]);

      nrf_radio_on();

      /* do we have a packet pending? */
      nrf_radio_pending_packet();

      nrf_radio_read(rxbuffer, 4);
      nrf_radio_off();
      printf ("PING\t RX: ----- Last packet: %u %u %u %02x\t\t\n\r",
	      rxbuffer[COUNT], rxbuffer[SENDER], rxbuffer[DELAY], rxbuffer[OPTIONAL]);

      //printf("RX:\t %u %u %u %u %u %u %u %u\n\r", rxbuffer[0], rxbuffer[1], rxbuffer[2], rxbuffer[3], rxbuffer[4], rxbuffer[5], rxbuffer[6], rxbuffer[7]);

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



      printf("Score --- A: %i - B: %u - X: %u\n\r", recvA, recvB, recvX);
      //rtimer_set(&rt, RTIMER_NOW()+RTIMER_ARCH_SECOND,1,send,NULL);


  }

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(blink_process, ev, data)
{
  PROCESS_BEGIN();

  blinks = 0;

  while(1) {
    etimer_set(&et_blink, CLOCK_SECOND/2);

    PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_TIMER);

    leds_off(LEDS_ALL);
    leds_on(blinks & LEDS_ALL);
    blinks++;
  }

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
