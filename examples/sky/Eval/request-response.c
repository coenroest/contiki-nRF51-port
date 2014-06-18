#include "contiki.h"
#include "net/rime/rime.h"
#include "net/netstack.h"
#include "simple-energest.h"

#include "dev/button-sensor.h"

#include "dev/leds.h"

#include <stdio.h>

#define SEND_INTERVAL   (10*CLOCK_SECOND)
#define PAYLOAD_SIZE    21

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
  if(linkaddr_cmp(&dst_addr, &linkaddr_node_addr)) {
    /* We are the destination, we send a response */
    //printf("app: request received, sending response\n");
    packetbuf_copyfrom(payload, PAYLOAD_SIZE);
    unicast_send(&uc, &src_addr);
  } else {
    /* We are the source, we got the response, record and print delay */
    clock_time_t response_time = clock_time();
    printf("app: response received, delay: %lu ms\n", ((uint32_t)(response_time - request_time) * 1000) / CLOCK_SECOND);
  }
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
  src_addr.u8[0] = 1;
  src_addr.u8[1] = 0;
  dst_addr.u8[0] = 2;
  dst_addr.u8[1] = 0;

  /* Set up timers and unicast socket */
  etimer_set(&periodic_timer, SEND_INTERVAL);
  unicast_open(&uc, 146, &unicast_callbacks);

  /* Start background process that will print out energy status periodically */
  simple_energest_start();

  printf("app: linkaddr %u %u\n", linkaddr_node_addr.u8[0], linkaddr_node_addr.u8[1]);

  /* We are the source */
  if(linkaddr_cmp(&src_addr, &linkaddr_node_addr)) {
    /* Disable duty cycling */
    NETSTACK_RDC.off(1);
	  while(1) {
	    /* Wait for random time within SEND_INTERVAL */
	    etimer_set(&send_timer, ((uint32_t)random_rand()) % SEND_INTERVAL);
	    PROCESS_WAIT_UNTIL(etimer_expired(&send_timer));

	    /* Send request */
	    printf("app: sending request\n");
		  packetbuf_copyfrom(payload, PAYLOAD_SIZE);
		  request_time = clock_time();
		  unicast_send(&uc, &dst_addr);

		  /* Wait for next interval */
		  PROCESS_WAIT_UNTIL(etimer_expired(&periodic_timer));
		  etimer_reset(&periodic_timer);
	  }
  }

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
