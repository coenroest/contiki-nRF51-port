/*
 * Author: PrithviRaj Narendra {prithvirajnarendra @ gmail.com}
 */

#include "contiki.h"
#include "stdint.h"
#include "dev/leds.h"
#include "softdevice_handler.h"
#include "nrf_soc.h"
#include "ble.h"
#include "nordic_common.h"
#include "ble_gap.h"
#include "app_error.h"
#include "app_util.h"
#include "ble_advdata.h"
#include "ble_srv_common.h"

#include <string.h>
#include <stdio.h> /* For printf() */
/*---------------------------------------------------------------------------*/
#define ADV_INTERVAL_IN_MS              1200
#define ADV_INTERVAL                    MSEC_TO_UNITS(ADV_INTERVAL_IN_MS, UNIT_0_625_MS) /**< The advertising interval (in units of 0.625 ms. */
#define ADV_TIMEOUT_IN_SECONDS          0                                           /**< The advertising timeout (in units of seconds). */
/*---------------------------------------------------------------------------*/
static void sys_evt_dispatch(uint32_t sys_evt);
static void ble_evt_dispatch(ble_evt_t * p_ble_evt);
void app_error_handler(uint32_t error_code, uint32_t line_num, const uint8_t * p_file_name);
void assert_nrf_callback(uint16_t line_num, const uint8_t * p_file_name);
/*---------------------------------------------------------------------------*/
static struct etimer et_hello;
static struct etimer et_blink;
static int32_t rand_val;
static uint8_t blinks;
/*---------------------------------------------------------------------------*/
PROCESS(hello_world_process, "Hello world process");
PROCESS(blink_process, "LED blink process");
AUTOSTART_PROCESSES(&hello_world_process, &blink_process);
/*---------------------------------------------------------------------------*/
static void ble_evt_dispatch(ble_evt_t * p_ble_evt)
{
}
/*---------------------------------------------------------------------------*/
static void sys_evt_dispatch(uint32_t sys_evt)
{
}
/*---------------------------------------------------------------------------*/
void app_error_handler(uint32_t error_code, uint32_t line_num, const uint8_t * p_file_name)
{
	printf("Error of %X at %d in %s", error_code, line_num, p_file_name);
	while(1){};
}
void assert_nrf_callback(uint16_t line_num, const uint8_t * p_file_name)
{
    app_error_handler(0xDEADBEEF, line_num, p_file_name);
}
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(hello_world_process, ev, data)
{
  PROCESS_BEGIN();

  uint32_t                err_code;

  // Initialize the SoftDevice handler module.
  SOFTDEVICE_HANDLER_INIT(NRF_CLOCK_LFCLKSRC_XTAL_20_PPM, false);
  // Register with the SoftDevice handler module for BLE events.
  err_code = softdevice_ble_evt_handler_set(ble_evt_dispatch);
  APP_ERROR_CHECK(err_code);
  // Register with the SoftDevice handler module for BLE events.
  err_code = softdevice_sys_evt_handler_set(sys_evt_dispatch);
  APP_ERROR_CHECK(err_code);

  ble_gap_conn_sec_mode_t sec_mode;
  char name_buffer[9];
  sprintf(name_buffer, "%08X",(unsigned int) NRF_FICR->DEVICEID[0]);
  BLE_GAP_CONN_SEC_MODE_SET_OPEN(&sec_mode);
  err_code = sd_ble_gap_device_name_set(&sec_mode,
                                        (const uint8_t *)name_buffer,
                                        strlen(name_buffer));
  APP_ERROR_CHECK(err_code);

  ble_advdata_t advdata;
  uint8_t       flags = BLE_GAP_ADV_FLAG_BR_EDR_NOT_SUPPORTED;
  ble_advdata_service_data_t service_data[2];
  uint8_t battery_data = 98;//battery_level_get();
  uint32_t temperature_data = 0xFE001213;
  service_data[0].service_uuid = BLE_UUID_BATTERY_SERVICE;
  service_data[0].data.size    = sizeof(battery_data);
  service_data[0].data.p_data  = &battery_data;
  service_data[1].service_uuid = BLE_UUID_HEALTH_THERMOMETER_SERVICE;
  service_data[1].data.size    = sizeof(temperature_data);
  service_data[1].data.p_data  = (uint8_t *) &temperature_data;
  // Build and set advertising data
  memset(&advdata, 0, sizeof(advdata));
  advdata.name_type            = BLE_ADVDATA_FULL_NAME;
  advdata.include_appearance   = false;
  advdata.flags.size           = sizeof(flags);
  advdata.flags.p_data         = &flags;
  advdata.service_data_count   = 2;
  advdata.p_service_data_array = service_data;
  err_code = ble_advdata_set(&advdata, NULL);
  APP_ERROR_CHECK(err_code);

  ble_gap_adv_params_t adv_params;
  // Start advertising
  memset(&adv_params, 0, sizeof(adv_params));
  adv_params.type        = BLE_GAP_ADV_TYPE_ADV_NONCONN_IND;
  adv_params.p_peer_addr = NULL;
  adv_params.fp          = BLE_GAP_ADV_FP_ANY;
  adv_params.interval    = ADV_INTERVAL;
  adv_params.timeout     = ADV_TIMEOUT_IN_SECONDS;
  err_code = sd_ble_gap_adv_start(&adv_params);
  APP_ERROR_CHECK(err_code);
  leds_off(LEDS_ALL);
  leds_on(LEDS_RED);

  PROCESS_PAUSE();
  etimer_set(&et_hello, CLOCK_SECOND/2);
  rand_val = 0;
  blinks = 0;

  while(1) {
    PROCESS_WAIT_EVENT();

    if(ev == PROCESS_EVENT_TIMER) {
    	sd_rand_application_vector_get(&blinks,1);
      printf("Sensor says #%X\n", (unsigned int) blinks);

      etimer_reset(&et_hello);
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
    etimer_set(&et_blink, CLOCK_SECOND);

    PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_TIMER);

//    leds_off(LEDS_ALL);
//    leds_on(blinks & LEDS_ALL);
    printf("Blink... (state %c)\n", leds_get());
  }

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
