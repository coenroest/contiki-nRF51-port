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
#include "sys/rtimer.h"

#include "nrf-radio.h"


#include <stdio.h> /* For printf() */
/*---------------------------------------------------------------------------*/
static struct etimer et_blink, et_tx;
static struct rtimer rt;

static uint8_t blinks;
static uint8_t txbuffer[4];  // Packet to transmit
static uint8_t rxbuffer[4];  // Packet to receive
rtimer_clock_t rtimer_ref_time, sched_ref_time;
static int count;


/*---------------------------------------------------------------------------*/
PROCESS(tx_process, "TX process");
PROCESS(blink_process, "LED blink process");
AUTOSTART_PROCESSES(&tx_process, &blink_process);
/*---------------------------------------------------------------------------*/
static void send(struct rtimer *rt, void *ptr) {

  //nrf_radio_send(txbuffer, sizeof(txbuffer));

  sched_ref_time = RTIMER_NOW();
  printf("RTimer sched time: %d \n\r", sched_ref_time);


  txbuffer[0] = count;
  count++;
  //nrf_radio_send(txbuffer, sizeof(txbuffer));
  NRF_RADIO->INTENSET = RADIO_INTENSET_ADDRESS_Msk;
}
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(tx_process, ev, data)
{
  PROCESS_BEGIN();

  count = 0;
  uint8_t btn0_state = nrf_gpio_pin_read(BUTTON_0);

  while(1)
  {
      etimer_set(&et_tx, CLOCK_SECOND);

      PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_TIMER);

      if (btn0_state == 1)
	{

	  nrf_radio_send(txbuffer, sizeof(txbuffer));

	  btn0_state = 0;
	}

      nrf_radio_read(rxbuffer, 4);
      printf("Contents of received packet: %d\n\r", (int)*rxbuffer);


      //rtimer_ref_time = RTIMER_NOW();
      rtimer_set(&rt, nrf_radio_read_sfd_timer()+RTIMER_ARCH_SECOND,1,send,NULL);
      printf("RTimer ref time: %d \n\r", nrf_radio_read_sfd_timer());
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
