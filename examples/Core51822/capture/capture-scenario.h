#ifndef CAPTURE_SCENARIO_H_
#define CAPTURE_SCENARIO_H_

#include <stdint.h>
#include "nrf51_bitfields.h"	/* Used for the specific power values */

#define SCENARIO 0
#define COUNT 1
#define SENDER 2
#define DELAY 3
#define MULT 4
#define	POWERA 5
#define POWERB 6
#define OPTIONAL2 7

/* FORMAT:
 *
 * Scenario, Count, Sender, Delay, Multiplier, Power, Optional1, Optional2
 *
 */

uint8_t scenario[7][8] =
	{
	    {0, 0, 8, 0, 0, RADIO_TXPOWER_TXPOWER_Pos4dBm, RADIO_TXPOWER_TXPOWER_Neg16dBm, 0},

	    {1, 0, 8, 128, 1, RADIO_TXPOWER_TXPOWER_Pos4dBm, RADIO_TXPOWER_TXPOWER_Neg16dBm, 0},

	    {2, 0, 8, 128, 1,  RADIO_TXPOWER_TXPOWER_Pos4dBm, RADIO_TXPOWER_TXPOWER_Neg20dBm, 0},

	    {3, 0, 8, 120, 1, RADIO_TXPOWER_TXPOWER_Pos4dBm, RADIO_TXPOWER_TXPOWER_Neg20dBm, 0},

	    {4, 0, 8, 64, 1, RADIO_TXPOWER_TXPOWER_Pos4dBm, RADIO_TXPOWER_TXPOWER_Neg16dBm, 0},

	    {5, 0, 8, 64, 1, RADIO_TXPOWER_TXPOWER_Pos4dBm, RADIO_TXPOWER_TXPOWER_Neg20dBm, 0},

	    {6, 0, 8, 64, 1, RADIO_TXPOWER_TXPOWER_Pos4dBm, RADIO_TXPOWER_TXPOWER_Neg30dBm, 0},
	};


#endif /*CAPTURE_SCENARIO_H_ */
