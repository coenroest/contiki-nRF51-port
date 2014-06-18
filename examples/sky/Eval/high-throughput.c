#include "contiki.h"
#include "net/rime/rime.h"
#include "net/netstack.h"
#include "simple-energest.h"

#include "dev/button-sensor.h"

#include "dev/leds.h"

#include <stdio.h>

#define SEND_INTERVAL   0
#define PAYLOAD_SIZE    110

static uint8_t payload[PAYLOAD_SIZE];
static linkaddr_t src_addr;
static linkaddr_t dst_addr;
static struct unicast_conn uc;
static clock_time_t request_time;

/*---------------------------------------------------------------------------*/
PROCESS(request_response_process, "Request-response process");
AUTOSTART_PROCESSES(&request_response_process);

/*---------------------------------------------------------------------------*/
static void
recv_uc(struct unicast_conn *c, const linkaddr_t *from)
{
  /* We got data */
  printf("app: packet received\n");
}

static const struct unicast_callbacks unicast_callbacks = {recv_uc};
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(request_response_process, ev, data)
{
  static struct etimer periodic_timer;
  static struct etimer send_timer;

  PROCESS_EXITHANDLER(unicast_close(&uc);)

  PROCESS_BEGIN();

  /* Hard-coded addresses */
  src_addr.u8[0] = 2;
  src_addr.u8[1] = 0;
  dst_addr.u8[0] = 1;
  dst_addr.u8[1] = 0;

  /* Set up timers and unicast socket */
  etimer_set(&periodic_timer, 4*CLOCK_SECOND);
  PROCESS_WAIT_UNTIL(etimer_expired(&periodic_timer));
  etimer_set(&periodic_timer, SEND_INTERVAL);
  unicast_open(&uc, 146, &unicast_callbacks);

  /* Start background process that will print out energy status periodically */
  simple_energest_start();

  printf("app: linkaddr %u %u\n", linkaddr_node_addr.u8[0], linkaddr_node_addr.u8[1]);

  /* We are the source */
  if(linkaddr_cmp(&src_addr, &linkaddr_node_addr)) {
	  while(1) {
	    /* Send request */
	    printf("app: sending packet\n");
	    packetbuf_copyfrom(payload, PAYLOAD_SIZE);
      unicast_send(&uc, &dst_addr);
		  /* Wait for next interval */
		  PROCESS_WAIT_UNTIL(etimer_expired(&periodic_timer));
		  etimer_reset(&periodic_timer);
	  }
  } else {
    /* Disable duty cycling */
    NETSTACK_RDC.off(1);
  }

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
