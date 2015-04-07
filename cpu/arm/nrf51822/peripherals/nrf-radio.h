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

int nrf_radio_fast_send(void);

int nrf_radio_receiving_packet(void);
int nrf_radio_pending_packet(void);

int nrf_radio_on(void);
int nrf_radio_off(void);

void nrf_radio_flushrx(void);

int nrf_radio_set_channel(int c);
int nrf_radio_get_channel(void);
int nrf_radio_rssi(void);
rtimer_clock_t nrf_radio_read_address_timestamp(void);

void RADIO_IRQHandler(void);

extern const struct radio_driver nrf_radio_driver;

int nrf_radio_set_txpower(int power);
int nrf_radio_get_txpower(void);
#define NRF_RADIO_TXPOWER_MAX  4
#define NRF_RADIO_TXPOWER_MIN  -30

// TODO CR use this for input check at set_txpower() ?
#define TXPOWER_ENUM(S) \
  ( (S==0) ? RADIO_TXPOWER_TXPOWER_Pos4dBm : \
    (S==1) ? RADIO_TXPOWER_TXPOWER_0dBm : \
    (S==2) ? RADIO_TXPOWER_TXPOWER_Neg4dBm : \
    (S==3) ? RADIO_TXPOWER_TXPOWER_Neg8dBm : \
    (S==4) ? RADIO_TXPOWER_TXPOWER_Neg12dBm : \
    (S==5) ? RADIO_TXPOWER_TXPOWER_Neg16dBm : \
    (S==6) ? RADIO_TXPOWER_TXPOWER_Neg20dBm : \
    (S==7) ? RADIO_TXPOWER_TXPOWER_Neg30dBm : RADIO_TXPOWER_TXPOWER_Pos4dBm)


/************************************************************************/
/* Generic names for special functions */
/************************************************************************/

//#define NETSTACK_RADIO_address_decode(E)        cc2420_address_decode((E))
//#define NETSTACK_RADIO_set_interrupt_enable(E)  cc2420_set_interrupt_enable((E))
//#define NETSTACK_RADIO_sfd_sync(S,E)            cc2420_sfd_sync((S),(E))
#define NETSTACK_RADIO_read_sfd_timer()         nrf_radio_read_address_timestamp()
#define NETSTACK_RADIO_set_channel(C)           nrf_radio_set_channel((C))
#define NETSTACK_RADIO_get_channel()            nrf_radio_get_channel()
#define NETSTACK_RADIO_radio_raw_rx_on()        nrf_radio_on();
#define NETSTACK_RADIO_set_txpower(X)           nrf_radio_set_txpower(X)
//#define NETSTACK_RADIO_set_cca_threshold(X)     cc2420_set_cca_threshold(X)
#define NETSTACK_RADIO_fast_send()     		nrf_radio_fast_send()
//#define NETSTACK_RADIO_rx_byte_available()     	cc2420_rx_byte_available()
//#define NETSTACK_RADIO_get_rx_byte(B)     	CC2420_GET_RX_BYTE((B))
#define NETSTACK_RADIO_flushrx()     		nrf_radio_flushrx()
//#define NETSTACK_RADIO_set_fifop(p)		cc2420_set_fifop((p))
//#define NETSTACK_RADIO_set_autoack(p)		cc2420_set_autoack((p))

#endif /* NRF_RADIO_H_ */
