

#include "contiki.h"
#include "dev/leds.h"
#include "dev/radio.h"
#include "simple-uart.h"
#include "nrf-radio.h"

#include <stdio.h> /* For printf() */
/*---------------------------------------------------------------------------*/

/* Parameters for sending a ping packet with controls */

#define DEVICE_ID 9

#define DELAY_TICKS 88

#define COUNT 0
#define SENDER 1
#define DELAY 2
#define OPTIONAL 3

/*---------------------------------------------------------------------------*/
static struct etimer et_blink, et_tx;
static struct rtimer rt;
static uint8_t blinks;
static uint8_t txbuffer[4];  ///< Packet to transmit

rtimer_clock_t rtimer_ref_time, after_blink;
static int count = 0;
static int recvA = 0;
static int recvB = 0;

/*---------------------------------------------------------------------------*/
PROCESS(ping_process, "Ping process");
PROCESS(blink_process, "LED blink process");
AUTOSTART_PROCESSES(&ping_process, &blink_process);
/*---------------------------------------------------------------------------*/
static void send(struct rtimer *rt, void *ptr) {

  printf("main-SEND\n\r");

  txbuffer[COUNT] = count++;//rxbuffer[COUNT];
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

      /* Switch the radio on and wait for incoming packets */
      //printf("------- RADIO ON ----- \n\r");

      txbuffer[COUNT] = count++;//rxbuffer[COUNT];
      txbuffer[SENDER] = DEVICE_ID;
      txbuffer[DELAY] = DELAY_TICKS;
      txbuffer[OPTIONAL] = 0x24;
      nrf_radio_send (txbuffer, 8);
      printf ("PING\t TX: ----- Packet: %u %u %u %02x\t\t timestamp: %u\n\r", txbuffer[COUNT],
    	      txbuffer[SENDER], txbuffer[DELAY], txbuffer[OPTIONAL], NRF_TIMER0->CC[TIMESTAMP_REG]);

      nrf_radio_on();

      /* do we have a packet pending? */
      while(NRF_RADIO->EVENTS_END == 0);
      /* we got something. clear event. */
      NRF_RADIO->EVENTS_END = 0;
      nrf_radio_read(rxbuffer, 8);
      printf ("PING\t RX: ----- Last packet: %u %u %u %02x\t\t timestamp: %u\n\r",
	      rxbuffer[COUNT], rxbuffer[SENDER], rxbuffer[DELAY], rxbuffer[OPTIONAL], NRF_TIMER0->CC[TIMESTAMP_REG]);

      if (rxbuffer[SENDER] == 1)
	{
	  recvA++;
	}
      else if (rxbuffer[SENDER] == 2)
	{
	  recvB++;
	}


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
