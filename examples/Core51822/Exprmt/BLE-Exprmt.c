/*
 * Author: PrithviRaj Narendra {prithvirajnarendra @ gmail.com}
 */

#include "contiki.h"
#include "stdint.h"
#include "Pillar-func.h"
#include "ble_l2cap.h"
#include <string.h>
#include <stdio.h> /* For printf() */
/*---------------------------------------------------------------------------*/
static uint16_t count_ab;
static struct etimer et_hello;
/*---------------------------------------------------------------------------*/
PROCESS(BLE_start, "BLE initiation process");
AUTOSTART_PROCESSES(&BLE_start);
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(BLE_start, ev, data)
{
  PROCESS_BEGIN();


	 // Initialize
	 ble_stack_init();

	 //buttons_init();
	 gap_params_init();
	 services_init();
	 advertising_init();
	 conn_params_init();
	 sec_params_init();

	 // Start execution
	 advertising_start();
	 l2cap_init();
	 printf("Start\n");

	 count_ab = 0;
	 etimer_set(&et_hello, CLOCK_SECOND);

	 while(1) {

	     PROCESS_YIELD();
	     if(ev == PROCESS_EVENT_TIMER) {
	       printf("%d\n", (int)count_ab);
	       count_ab++;

	       etimer_reset(&et_hello);
	     }
	   }

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
