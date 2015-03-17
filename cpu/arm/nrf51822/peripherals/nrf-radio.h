#ifndef NRF_RADIO_H_
#define NRF_RADIO_H_

#include "contiki.h"
#include "dev/radio.h"

int nrf_radio_init(void);

int nrf_radio_prepare(const void *payload, unsigned short payload_len);
int nrf_radio_transmit(unsigned short transmit_len);
int nrf_radio_send(const void *payload, unsigned short payload_len);

int nrf_radio_read(void *buf, unsigned short buf_len);

int nrf_radio_on(void);
int nrf_radio_off(void);

int nrf_radio_set_channel(int channel);

int nrf_radio_fast_send(void);

void RADIO_IRQHandler(void);


extern const struct radio_driver nrf_radio_driver;

#endif /* NRF_RADIO_H_ */
