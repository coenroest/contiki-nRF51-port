#ifndef PROJECT_CONF_H_
#define PROJECT_CONF_H_

/* A very insisting Csma layer with many retransmissions */
#undef CSMA_CONF_MAX_MAC_TRANSMISSIONS
#define CSMA_CONF_MAX_MAC_TRANSMISSIONS 32

#undef NETSTACK_CONF_MAC
/* For request response case */
//#define NETSTACK_CONF_MAC     csma_driver
/* For high throughput case */
#define NETSTACK_CONF_MAC     csmafast_driver

#undef NETSTACK_CONF_RDC
/* No duty cycling */
//#define NETSTACK_CONF_RDC     nullrdc_driver
/* Duty cycling with ContikiMAC */
//#define NETSTACK_CONF_RDC     contikimac_driver
/* For high throughput case */
#define NETSTACK_CONF_RDC     nullrdcfast_driver

/* Enable link-layer ACK */
#undef NULLRDC_CONF_802154_AUTOACK
#define NULLRDC_CONF_802154_AUTOACK 1

/* ContikiMAC wakeup rate. Default is 8 Hz (period of 125ms) */
#undef NETSTACK_RDC_CHANNEL_CHECK_RATE
#define NETSTACK_RDC_CHANNEL_CHECK_RATE 8

/* For ContikiMAC. We disable phase lock for improved
 * latency towards always-on nodes. */
#undef CONTIKIMAC_CONF_WITH_PHASE_OPTIMIZATION
#define CONTIKIMAC_CONF_WITH_PHASE_OPTIMIZATION 0

#undef CLOCK_CONF_SECOND
#define CLOCK_CONF_SECOND 2048UL

#undef CC2420_CONF_CHANNEL
#define CC2420_CONF_CHANNEL 15

#endif /* PROJECT_CONF_H_ */
