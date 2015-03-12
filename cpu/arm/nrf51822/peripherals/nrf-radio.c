/**
 * \file
 *         nRF51 radio driver
 * \author
 *         Coen Roest <coen@student.chalmers.se>
 *
 */

#include <string.h>
#include <inttypes.h>

#include "contiki.h"
#include "nrf-radio.h"

#include "net/packetbuf.h"
#include "net/netstack.h"

#define DEBUG 1
#if DEBUG
#define PRINTF(...) printf(__VA_ARGS__)
#else
#define PRINTF(...) do {} while (0)
#endif

#define NRF51_RADIO_MAX_PACKET_LEN 4 		/* TODO: find max value */

#define RADIO_SHORTS_ENABLED 1

/*---------------------------------------------------------------------------*/
PROCESS(nrf_radio_process, "nRF Radio driver");
/*---------------------------------------------------------------------------*/

int nrf_radio_init(void);

int nrf_radio_prepare(const void *payload, unsigned short payload_len);
int nrf_radio_transmit(unsigned short transmit_len);
int nrf_radio_send(const void *payload, unsigned short payload_len);

int nrf_radio_read(void *buf, unsigned short buf_len);

int nrf_radio_on(void);
int nrf_radio_off(void);

int nrf_radio_set_channel(int channel);

int nrf_radio_fast_send(void);
int nrf_radio_capture_sfd_time(void);

//void ppi_init(void);


static uint8_t packet_ptr[4];  /* Pointer for receiving and transmitting */

const struct radio_driver nrf_radio_driver =
{
    nrf_radio_init,
    nrf_radio_prepare,
    nrf_radio_transmit,
    nrf_radio_send,
    nrf_radio_read,
    nrf_radio_set_channel,
    nrf_radio_fast_send,
    nrf_radio_capture_sfd_time,
    /* detected_energy, */
    //nrf_radio_cca,
    //nrf_radio_receiving_packet,
    //nrf_radio_pending_packet,
    nrf_radio_on,
    nrf_radio_off,
};

/*---------------------------------------------------------------------------*/
static uint8_t locked, lock_on, lock_off;
#define GET_LOCK() locked++
static void RELEASE_LOCK(void) {
  if(locked == 1) {
    if(lock_on) {
      //on();
      lock_on = 0;
    }
    if(lock_off) {
      //off();
      lock_off = 0;
    }
  }
  locked--;
}
/*---------------------------------------------------------------------------*/
#define PACKET0_S1_SIZE                  (0UL)  //!< S1 size in bits
#define PACKET0_S0_SIZE                  (0UL)  //!< S0 size in bits
#define PACKET0_PAYLOAD_SIZE             (0UL)  //!< payload size (length) in bits
#define PACKET1_BASE_ADDRESS_LENGTH      (4UL)  //!< base address length in bytes
#define PACKET1_STATIC_LENGTH            (1UL)  //!< static length in bytes
#define PACKET1_PAYLOAD_SIZE             (1UL)  //!< payload size in bytes
/*---------------------------------------------------------------------------*/
int
nrf_radio_init(void)
{
    if(locked) {
	return 0;
    }
    GET_LOCK();

    /* Reset all states in the radio peripheral */
    NRF_RADIO->POWER = 0;
    NRF_RADIO->POWER = 1;

    /* Radio config */
    NRF_RADIO->TXPOWER = (RADIO_TXPOWER_TXPOWER_Pos4dBm << RADIO_TXPOWER_TXPOWER_Pos);
    nrf_radio_set_channel(40UL);	// Frequency bin 40, 2440MHz
    NRF_RADIO->MODE = (RADIO_MODE_MODE_Nrf_2Mbit << RADIO_MODE_MODE_Pos);

    /* Radio address config */
    //NRF_RADIO->PREFIX0 = 0xC4C3C2E7UL;  // Prefix byte of addresses 3 to 0
    //NRF_RADIO->PREFIX1 = 0xC5C6C7C8UL;  // Prefix byte of addresses 7 to 4
    //NRF_RADIO->BASE0   = 0xE7E7E7E7UL;  // Base address for prefix 0
    //NRF_RADIO->BASE1   = 0x00C2C2C2UL;  // Base address for prefix 1-7

    NRF_RADIO->BASE0 = 0x42;
    NRF_RADIO->PREFIX0 = RADIO_PREFIX0_AP0_Msk; //0x42;

    NRF_RADIO->TXADDRESS = 0x00UL;      // Set device address 0 to use when transmitting
    NRF_RADIO->RXADDRESSES = 0x01UL;    // Enable device address 0 to use which receiving

    /* Packet configuration */
    NRF_RADIO->PCNF0 = (PACKET0_S1_SIZE << RADIO_PCNF0_S1LEN_Pos) |
		       (PACKET0_S0_SIZE << RADIO_PCNF0_S0LEN_Pos) |
		       (PACKET0_PAYLOAD_SIZE << RADIO_PCNF0_LFLEN_Pos); //lint !e845 "The right argument to operator '|' is certain to be 0"

    /* Packet configuration */
     NRF_RADIO->PCNF1 = (RADIO_PCNF1_WHITEEN_Disabled << RADIO_PCNF1_WHITEEN_Pos)    |
			(RADIO_PCNF1_ENDIAN_Big << RADIO_PCNF1_ENDIAN_Pos)           |
			(PACKET1_BASE_ADDRESS_LENGTH << RADIO_PCNF1_BALEN_Pos)       |
			(PACKET1_STATIC_LENGTH << RADIO_PCNF1_STATLEN_Pos)           |
			(PACKET1_PAYLOAD_SIZE << RADIO_PCNF1_MAXLEN_Pos); //lint !e845 "The right argument to operator '|' is certain to be 0"

    /* CRC Config */
    NRF_RADIO->CRCCNF = (RADIO_CRCCNF_LEN_Two << RADIO_CRCCNF_LEN_Pos); // Number of checksum bits
    if ((NRF_RADIO->CRCCNF & RADIO_CRCCNF_LEN_Msk) == (RADIO_CRCCNF_LEN_Two << RADIO_CRCCNF_LEN_Pos))
    {
      NRF_RADIO->CRCINIT = 0xFFFFUL;      // Initial value
      NRF_RADIO->CRCPOLY = 0x11021UL;     // CRC poly: x^16+x^12^x^5+1
    }
    else if ((NRF_RADIO->CRCCNF & RADIO_CRCCNF_LEN_Msk) == (RADIO_CRCCNF_LEN_One << RADIO_CRCCNF_LEN_Pos))
    {
      NRF_RADIO->CRCINIT = 0xFFUL;        // Initial value
      NRF_RADIO->CRCPOLY = 0x107UL;       // CRC poly: x^8+x^2^x^1+1
    }

    /* Config Shortcuts like in page 86 and 88 of nRF series ref man */
#if RADIO_SHORTS_ENABLED
    NRF_RADIO->SHORTS = (RADIO_SHORTS_READY_START_Enabled << RADIO_SHORTS_READY_START_Pos) |
			(RADIO_SHORTS_END_DISABLE_Enabled << RADIO_SHORTS_END_DISABLE_Pos);
#endif

    // PPI address -> timer0 enable
    NRF_PPI->CHEN = (PPI_CHEN_CH26_Enabled << PPI_CHEN_CH26_Pos);

    /* Set the packet pointer */
    NRF_RADIO->PACKETPTR = (uint32_t)packet_ptr;

    RELEASE_LOCK();

    process_start(&nrf_radio_process, NULL);

    return 1;
}
/*---------------------------------------------------------------------------*/
int
nrf_radio_prepare(const void *payload, unsigned short payload_len)
{
  /* Copy the payload to the location assigned to packet pointer */

  GET_LOCK();

  /* Switch the packet pointer to the payload */
  NRF_RADIO->PACKETPTR = (uint32_t)payload;

  RELEASE_LOCK();
  return 1;

}
/*---------------------------------------------------------------------------*/
int
nrf_radio_transmit(unsigned short transmit_len)
{

  GET_LOCK();
  NRF_RADIO->TASKS_TXEN = 1;	/* With shortcuts enabled, this is the only command needed */

  NRF_RADIO->EVENTS_END = 0U;  		/* Make sure the radio has finished transmitting */
  while(NRF_RADIO->EVENTS_END == 0U)
  {
  }
  RELEASE_LOCK();
  PRINTF("PACKET SEND\n\r");
  return 1;
}
/*---------------------------------------------------------------------------*/
int
nrf_radio_send(const void *payload, unsigned short payload_len)
{
  nrf_radio_prepare(payload, payload_len);
  return nrf_radio_transmit(payload_len);
}
/*---------------------------------------------------------------------------*/
int
nrf_radio_read(void *buf, unsigned short buf_len)
{
  GET_LOCK();
  int ret = 0;

  NRF_RADIO->TASKS_RXEN = 1U; /* With shortcuts enabled, this is the only command needed */

  if (NRF_RADIO->CRCSTATUS == RADIO_CRCSTATUS_CRCSTATUS_CRCOk)
  {
      PRINTF("PACKET RECEIVED\n\r");
      //memcpy(buf, packet_ptr, buf_len);		/* Place the contents in the read buffer */

      /* Switch the packet pointer to the payload */
      NRF_RADIO->PACKETPTR = (uint32_t)buf;
      ret = sizeof(buf);			/* Fix me: find actual size of packet */
  }

  else if(NRF_RADIO->CRCSTATUS == RADIO_CRCSTATUS_CRCSTATUS_CRCError)
  {
      PRINTF("PACKET RECEIVE FAILED\n\r");
  }

  RELEASE_LOCK();
  return ret;
}
/*---------------------------------------------------------------------------*/
int
nrf_radio_on(void)
{

  /* Difficult since there are is no global ON state,
   * only commands to set the radio in TX or RX mode.
   */

  return 1;
}
/*---------------------------------------------------------------------------*/
int
nrf_radio_off(void)
{
  /* Clear event register */
  NRF_RADIO->EVENTS_DISABLED = 0U;
  /* Disable radio */
  NRF_RADIO->TASKS_DISABLE = 1U;
  while(NRF_RADIO->EVENTS_DISABLED == 0U)
  {
      /* Wait for the radio to turn off */
  }

  return 1;
}
/*---------------------------------------------------------------------------*/
int
nrf_radio_set_channel(int channel)
{
  if (channel < 0 || channel > 100)
  {
    PRINTF("Channel NOT set!\n\r");
    return 0;
  }

  NRF_RADIO->FREQUENCY = (uint8_t)channel;
  return 1;
}
/*---------------------------------------------------------------------------*/
int
nrf_radio_fast_send(void)
{
  /*
   * This function can only be used if the shortcuts are disabled and
   * if the radio is already ramped up.
   * It will send what is in the packet buffer at that particular moment.
   */

  if(locked) {
    return 0;
  }
  GET_LOCK();


#if RADIO_SHORTS_ENABLED

  if(NRF_RADIO->STATE == RADIO_STATE_STATE_TxIdle)
    {
      NRF_RADIO->TASKS_START;
      PRINTF("Packet fast send finished\n\r");
      return 1;
    }

#endif

  PRINTF("Packet fast send failed\n\r");
  return 0;
}
/*---------------------------------------------------------------------------*/
int
nrf_radio_capture_sfd_time(void)
{
  /* TODO: does this work properly? */
  NRF_TIMER0->TASKS_CAPTURE[0] = 1;
  uint32_t time = NRF_TIMER0->CC[0];

  // Maybe clear register and reset the timer here?
  return time;
}
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(nrf_radio_process, ev, data)
{
  int len;
  PROCESS_BEGIN();

  PRINTF("nrf_radio_process: started\n\r");

  while(1) {
    PROCESS_YIELD_UNTIL(ev == PROCESS_EVENT_POLL);

    PRINTF("nrf_radio_process: calling receiver callback\n\r");

    packetbuf_clear();
    //packetbuf_set_attr(PACKETBUF_ATTR_TIMESTAMP, last_packet_timestamp);
    len = nrf_radio_read(packetbuf_dataptr(), PACKETBUF_SIZE);

    packetbuf_set_datalen(len);

    NETSTACK_RDC.input();
  }

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/

