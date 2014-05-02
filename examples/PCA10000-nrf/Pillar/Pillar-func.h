/*
 * Pillar-func.h
 *
 *  Created on: 29-Apr-2014
 *      Author: prithvi
 */

#ifndef PILLAR_FUNC_H_
#define PILLAR_FUNC_H_

// Persistent storage system event handler
void pstorage_sys_event_handler (uint32_t p_evt);

void app_error_handler(uint32_t error_code, uint32_t line_num, const uint8_t * p_file_name);

void assert_nrf_callback(uint16_t line_num, const uint8_t * p_file_name);

void uart_init();

void gap_params_init(void);

void advertising_init(void);

void services_init(void);

void sec_params_init(void);

void conn_params_init(void);

void led_timer_start(void);

void led_timer_stop(void);

void alarm_timer_start(uint32_t counts);

void advertising_start(void);

void ble_stack_init(void);

#endif /* PILLAR_FUNC_H_ */
