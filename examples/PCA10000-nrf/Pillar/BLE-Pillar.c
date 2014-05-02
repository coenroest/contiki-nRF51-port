/*
 * Author: PrithviRaj Narendra {prithvirajnarendra @ gmail.com}
 */

#include "contiki.h"
#include "stdint.h"
#include "Pillar-func.h"
#include <string.h>
#include <stdio.h> /* For printf() */
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
	 printf("Start\n");

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
