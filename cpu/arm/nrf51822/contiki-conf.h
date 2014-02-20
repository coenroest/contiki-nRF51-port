/*
 * contiki-conf.h
 *
 *  Created on: 30-Jan-2014
 *      Author: prithvi
 */

#ifndef CONTIKI_CONF_H_
#define CONTIKI_CONF_H_

#include <stdint.h>
#include "nrf.h"

#define CCIF
#define CLIF

/* For the main clock. RTC1 is used in nrf51822*/
#define CLOCK_CONF_SECOND 64

/* Defined as 0 for UART0 and 1 for UART1 */
#define DEBUG_UART      0

typedef uint32_t clock_time_t;

#if 0
typedef unsigned int uip_stats_t;

#ifndef BV
#define BV(x) (1<<(x))
#endif

/* Define the MAC address of the device */
#define EMAC_ADDR0              0x10
#define EMAC_ADDR1              0x1F
#define EMAC_ADDR2              0xE0
#define EMAC_ADDR3              0x12
#define EMAC_ADDR4              0x1D
#define EMAC_ADDR5              0x0C

/* uIP configuration */
/* Ethernet LLH(Link Level Header) size is 14 bytes */
#define UIP_CONF_LLH_LEN	14
#define UIP_CONF_BROADCAST	1
#define UIP_CONF_LOGGING	1
#define UIP_CONF_BUFFER_SIZE	1024
#define UIP_CONF_TCP_FORWARD	1
#define UIP_CONF_ICMP6		1
#define UIP_CONF_LL_802154	1

/* Prefix for relocation sections in ELF files */
#define REL_SECT_PREFIX ".rel"

#define CC_BYTE_ALIGNED __attribute__ ((packed, aligned(1)))

#define USB_EP1_SIZE 64
#define USB_EP2_SIZE 64

#define RAND_MAX 0x7fff
#endif

#endif /* CONTIKI_CONF_H_ */
