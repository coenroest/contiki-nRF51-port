/**
 * \addtogroup nrf51822
 * @{
 * \defgroup nrf51-radio-driver Contiki radio driver implementation for nrf51822 SoC
 * 			This module contains the radio driver implementation for nrf51822 SoC
 *
 * @{
 * \file
 * nrf51822 architecture specific declarations for the nRF51 radio
 * \author CoenRoest
 */
#ifndef NRF_RADIO_H_
#define NRF_RADIO_H_

#include "contiki.h"
#include "dev/radio.h"

int nrf_radio_init(void);

int nrf_radio_prepare(const void *payload, unsigned short payload_len);
int nrf_radio_transmit(unsigned short transmit_len);
int nrf_radio_send(const void *payload, unsigned short payload_len);

int nrf_radio_read(void *buf, unsigned short buf_len);

int nrf_radio_receiving_packet(void);
int nrf_radio_pending_packet(void);

int nrf_radio_on(void);
int nrf_radio_off(void);

int nrf_radio_set_channel(int channel);
int nrf_radio_set_txpower(int power);


int nrf_radio_fast_send(void);

rtimer_clock_t nrf_radio_read_address_timestamp(void);

void RADIO_IRQHandler(void);

extern const struct radio_driver nrf_radio_driver;

#endif /* NRF_RADIO_H_ */
