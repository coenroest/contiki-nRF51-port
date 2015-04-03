/**
 * \addtogroup nrf51-radio-driver
 * @{
 * \file
 * nrf51822 radio driver
 *  \author CoenRoest
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

/*---------------------------------------------------------------------------*/

#ifndef RADIO_SHORTS_ENABLED
#define RADIO_SHORTS_ENABLED true
#endif

#ifndef RADIO_BCC_ENABLED
#define RADIO_BCC_ENABLED false
#endif

#ifndef RADIO_INTERRUPT_ENABLED
#define RADIO_INTERRUPT_ENABLED false
#endif

#ifndef RADIO_RSSI_ENABLED
#define RADIO_RSSI_ENABLED true
#endif

/*---------------------------------------------------------------------------*/
PROCESS(nrf_radio_process, "nRF Radio driver");
/*---------------------------------------------------------------------------*/

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

int nrf_radio_rssi(void);

/* Local functions prototypes */
static void on(void);
static void off(void);

const struct radio_driver nrf_radio_driver =
{
    nrf_radio_init,
    nrf_radio_prepare,
    nrf_radio_transmit,
    nrf_radio_send,
    nrf_radio_read,
    //nrf_radio_set_channel,
    //nrf_radio_set_txpower,
    nrf_radio_fast_send,
    /* detected_energy, */
    //nrf_radio_cca,
    nrf_radio_receiving_packet,
    nrf_radio_pending_packet,
    //nrf_radio_read_address_timestamp,
    nrf_radio_on,
    nrf_radio_off,
};

/*---------------------------------------------------------------------------*/
static uint8_t locked, lock_on, lock_off;
static uint8_t receive_on;

#define GET_LOCK() locked++
static void RELEASE_LOCK(void) {
  if(locked == 1) {
    if(lock_on) {
      //TODO CR why is this needed or not? Test it!
      //on();
      lock_on = 0;
    }
    if(lock_off) {
      //TODO CR why is this needed or not? Test it!
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
#define PACKET1_STATIC_LENGTH            (8UL)  //!< static length in bytes
#define PACKET1_PAYLOAD_SIZE             (8UL)  //!< payload size in bytes

uint8_t nrf_buffer[PACKET1_PAYLOAD_SIZE];  ///< buffer for packets
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
    nrf_radio_set_txpower(RADIO_TXPOWER_TXPOWER_Pos4dBm);
    nrf_radio_set_channel(40UL);	// Frequency bin 40, 2440MHz
    NRF_RADIO->MODE = (RADIO_MODE_MODE_Ble_1Mbit << RADIO_MODE_MODE_Pos);

    //NRF_RADIO->BASE0 = 0x42424242;
    //NRF_RADIO->BASE0 = 0x8E89BED6;	// BLE Advertising address
    NRF_RADIO->BASE0 = 0x8E89BED5;

    // Radio address config
/*
    NRF_RADIO->PREFIX0 = 0xC4C3C2E7UL;  // Prefix byte of addresses 3 to 0
    NRF_RADIO->PREFIX1 = 0xC5C6C7C8UL;  // Prefix byte of addresses 7 to 4
    NRF_RADIO->BASE0   = 0xE7E7E7E7UL;  // Base address for prefix 0
    NRF_RADIO->BASE1   = 0x00C2C2C2UL;  // Base address for prefix 1-7
*/

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

     /* Configure buffer */
     NRF_RADIO->PACKETPTR = (uint32_t)nrf_buffer;

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
  NRF_RADIO->SHORTS |= 	(RADIO_SHORTS_READY_START_Enabled << RADIO_SHORTS_READY_START_Pos) |
			(RADIO_SHORTS_END_DISABLE_Enabled << RADIO_SHORTS_END_DISABLE_Pos);
#endif

#if RADIO_BCC_ENABLED
  NRF_RADIO->SHORTS |= RADIO_SHORTS_ADDRESS_BCSTART_Enabled << RADIO_SHORTS_ADDRESS_BCSTART_Pos;
  /* How many bits do we want to count? */
  NRF_RADIO->BCC = 24;
#endif

#if RADIO_RSSI_ENABLED
  NRF_RADIO->SHORTS |= RADIO_SHORTS_ADDRESS_RSSISTART_Msk;
  NRF_RADIO->SHORTS |= RADIO_SHORTS_DISABLED_RSSISTOP_Msk;
#endif

#if RADIO_INTERRUPT_ENABLED

  /* Enable interrupts for specific events */
  NRF_RADIO->INTENSET |= RADIO_INTENSET_ADDRESS_Msk;
  NRF_RADIO->INTENSET |= RADIO_INTENSET_END_Msk;
  NRF_RADIO->INTENSET |= RADIO_INTENSET_BCMATCH_Msk;

  NVIC_SetPriority (RADIO_IRQn, 10);
  NVIC_ClearPendingIRQ (RADIO_IRQn);
  NVIC_EnableIRQ (RADIO_IRQn);

#endif

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
  //NRF_RADIO->PACKETPTR = (uint32_t)payload;

  /* Reset the contents of nrf_buffer */
  memset(nrf_buffer, 0, payload_len);

  memcpy(nrf_buffer, (uint32_t*)payload, payload_len);

  RELEASE_LOCK();
  return 1;

}
/*---------------------------------------------------------------------------*/
int
nrf_radio_transmit(unsigned short transmit_len)
{
  if(locked) {
    return 0;
  }
  GET_LOCK();


#if ! RADIO_INTERRUPT_ENABLED			/* Transmitting without interrupts */

  NRF_RADIO->EVENTS_END = 0U;			/* Clear the end event register */

  if (RADIO_SHORTS_ENABLED)
    {
      NRF_RADIO->TASKS_TXEN = 1;		/* Enable the radio in TX mode and start sending */
    }
  else
    {
      NRF_RADIO->EVENTS_READY = 0;		/* Clear ready register */
      NRF_RADIO->TASKS_TXEN = 1;		/* Enable the radio in TX mode */
      while(NRF_RADIO->EVENTS_READY == 0U);	/* Wait for the radio to ramp up */
      NRF_RADIO->TASKS_START = 1;		/* Start the transmission */
    }

  while(NRF_RADIO->EVENTS_END == 0U);		/* Wait for the transmission to finish */

  if (! RADIO_SHORTS_ENABLED)
    {
      NRF_RADIO->EVENTS_DISABLED = 0U;		/* Clear the disable event register */
      NRF_RADIO->TASKS_DISABLE = 1U;		/* Disable the radio */
      while (NRF_RADIO->EVENTS_DISABLED == 0U);	/* Wait for disabling to finish */
    }
#endif

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
  if(locked) {
    return 0;
  }
  GET_LOCK();
  int ret = 0;

  if (NRF_RADIO->CRCSTATUS == RADIO_CRCSTATUS_CRCSTATUS_CRCOk)
  {
      PRINTF("PACKET RECEIVED\n\r");

      /* Reset the contents of buf */
      memset(buf, 0, buf_len);

      /* Copy contents of the nrf_buffer to buf */
      memcpy(buf, (const char *) (nrf_buffer), buf_len);

      ret = sizeof(buf);			/* Fix me: find actual size of packet */
  }

  else if(NRF_RADIO->CRCSTATUS == RADIO_CRCSTATUS_CRCSTATUS_CRCError)
  {
      PRINTF("PACKET RECEIVE FAILED\n\r");
      /* Set the buf to error */
      memset(buf, 6, buf_len);
  }
/*  NRF_RADIO->EVENTS_END = 0U;  Make sure the radio has finished receiving
  while (NRF_RADIO->EVENTS_END == 0U)
    {
    }*/
  RELEASE_LOCK();
  return ret;
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
nrf_radio_set_txpower(int power)
{

  /* TODO CR: make an input check here */

  NRF_RADIO->TXPOWER = power << RADIO_TXPOWER_TXPOWER_Pos;
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

  if(!RADIO_SHORTS_ENABLED)
  {
    if(NRF_RADIO->STATE == RADIO_STATE_STATE_TxIdle)
    {
    NRF_RADIO->TASKS_START = 1;
    PRINTF("Packet fast send finished\n\r");
    RELEASE_LOCK();
    return 1;
    }
  }

  PRINTF("Packet fast send failed\n\r");
  RELEASE_LOCK();
  return 0;
}
/*---------------------------------------------------------------------------*/
int
nrf_radio_receiving_packet(void)
{
  if (! RADIO_INTERRUPT_ENABLED)
    {
      /*
       * Poll for ADDRESS event
       *
       * The register must be cleared beforehand in radio_on()
       */
      while (NRF_RADIO->EVENTS_ADDRESS == 0);
      return 1;
    }
  else
    {
      /* Set a flag from the interrupt handler? */
      return 0;
    }
  }
/*---------------------------------------------------------------------------*/
int
nrf_radio_pending_packet(void)
{
  if (! RADIO_INTERRUPT_ENABLED)
    {
      /*
       * Poll for END event
       *
       * The register must be cleared beforehand in radio_on()
       */
      while (NRF_RADIO->EVENTS_END == 0);
      return 1;
    }
  else
    {
      /* Set a flag from the interrupt handler? */
      return 0;
    }
}
/*---------------------------------------------------------------------------*/
rtimer_clock_t
nrf_radio_read_address_timestamp(void)
{
  /* Read the last address timestamp from the TIMER0 capture register */
  return NRF_TIMER0->CC[TIMESTAMP_REG];
}
/*---------------------------------------------------------------------------*/
int
nrf_radio_on(void)
{
  if(receive_on) {
      return 1;
    }
    if(locked) {
      lock_on = 1;
      return 1;
    }

    GET_LOCK();
    on();
    RELEASE_LOCK();
    return 1;

}
/*---------------------------------------------------------------------------*/
int
nrf_radio_off(void)
{
  /* Don't do anything if we are already turned off. */
  if(receive_on == 0) {
    return 1;
  }

  /* If we are called when the driver is locked, we indicate that the
     radio should be turned off when the lock is unlocked. */
  if(locked) {
    /*    printf("Off when locked (%d)\n", locked);*/
    lock_off = 1;
    return 1;
  }

  GET_LOCK();
  off();
  RELEASE_LOCK();
  return 1;
}
/*---------------------------------------------------------------------------*/
void
on(void)
{

  if (! RADIO_INTERRUPT_ENABLED)
      {
        /* Clear ADDRESS register
         *
         * Is this needed for receiving_packet() function? */
        NRF_RADIO->EVENTS_ADDRESS = 0U;

        /* Clear END register
         *
         * Is this needed for pending_packet() function? */
        NRF_RADIO->EVENTS_END = 0U;
      }

    if (RADIO_SHORTS_ENABLED)
      {
        NRF_RADIO->TASKS_RXEN = 1U;		/* Enable the radio in RX mode, radio will do the rest */
      }
    else
      {
        NRF_RADIO->EVENTS_READY = 0U;		/* Clear ready register */
        NRF_RADIO->TASKS_RXEN = 1U;		/* Enable the radio in RX mode */
        while(NRF_RADIO->EVENTS_READY == 0U);	/* Wait for the radio to ramp up */
        NRF_RADIO->TASKS_START = 1U;		/* Start the reception */
      }

    receive_on = 1;
}
/*---------------------------------------------------------------------------*/
void
off(void)
{
  /*  PRINTF("off\n");*/
  receive_on = 0;

  /* Clear event register */
  NRF_RADIO->EVENTS_DISABLED = 0U;
  /* Disable radio */
  NRF_RADIO->TASKS_DISABLE = 1U;
  while(NRF_RADIO->EVENTS_DISABLED == 0U)
  {
      /* Wait for the radio to turn off */
  }
}
/*---------------------------------------------------------------------------*/
int
nrf_radio_rssi(void)
{
  int rssi = 0;

  rssi = NRF_RADIO->RSSISAMPLE;

  return rssi;
}
/*---------------------------------------------------------------------------*/
void
RADIO_IRQHandler(void)
{
/*

  if (NRF_RADIO->EVENTS_ADDRESS == 1)
    {
      NRF_RADIO->EVENTS_ADDRESS = 0;
       Clear the interrupt register
      NRF_RADIO->INTENCLR = RADIO_INTENCLR_ADDRESS_Clear
	  << RADIO_INTENCLR_ADDRESS_Pos;
      PRINTF("INTERRUPTED! \n\r");
      if (NRF_RADIO->STATE == RADIO_STATE_STATE_RxIdle ||
	  NRF_RADIO->STATE == RADIO_STATE_STATE_Rx)
	{
	  PRINTF("INTERRUPTED (RECEIVING)! \n\r");
	}
      NRF_RADIO->INTENSET |= RADIO_INTENSET_ADDRESS_Msk;
    }


  if (NRF_RADIO->EVENTS_END == 1)
    {
       Clear the interrupt and event register
      NRF_RADIO->EVENTS_END = 0;
      NRF_RADIO->INTENCLR = RADIO_INTENCLR_END_Clear
	  << RADIO_INTENCLR_ADDRESS_Pos;

      //PRINTF("END - - interrupt!\t state: %u \n\r", NRF_RADIO->STATE);
      if (NRF_RADIO->STATE == RADIO_STATE_STATE_RxIdle ||
	  NRF_RADIO->STATE == RADIO_STATE_STATE_RxDisable ||
	  NRF_RADIO->STATE == RADIO_STATE_STATE_Disabled ||
	  NRF_RADIO->STATE == RADIO_STATE_STATE_Rx)

	{
	  PRINTF("END - - INTERRUPTED (RECEIVING)! \n\r");
	  process_poll(&nrf_radio_process);
	}


      //NRF_RADIO->INTENSET |= RADIO_INTENSET_END_Msk;
    }


  if (NRF_RADIO->EVENTS_BCMATCH == 1)
    {
       Clear the interrupt register
      NRF_RADIO->INTENCLR = RADIO_INTENCLR_BCMATCH_Clear << RADIO_INTENCLR_BCMATCH_Pos;

       Clear the event register
      NRF_RADIO->EVENTS_BCMATCH = 0;

       Read out the capture registers of the Address event and the BCMatch event
      time = NRF_TIMER0->CC[BCC_REG];
      ref_time = NRF_TIMER0->CC[TIMESTAMP_REG];

       Disable the Bit counter, it will be restarted by the shortcut
       * between Address event and the BCStart task.

      NRF_RADIO->TASKS_BCSTOP;

       Re-enable the interrupt
      NRF_RADIO->INTENSET = RADIO_INTENSET_BCMATCH_Msk;

      PRINTF("BC MATCH! \t\t Measured timer ticks: %u -----\n\r", (time - ref_time));
    }
*/

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
    //len = nrf_radio_read(packetbuf_dataptr(), PACKETBUF_SIZE);

    //nrf_radio_read(rxbuffer, 8);

    //packetbuf_set_datalen(len);

    NETSTACK_RDC.input();
  }

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/

