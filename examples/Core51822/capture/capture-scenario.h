#ifndef CAPTURE_SCENARIO_H_
#define CAPTURE_SCENARIO_H_

#include <stdint.h>

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

uint8_t scenario[6][8] =
	{
	    {0, 0, 8, 0, 0, 0, 0, 0},

	    {1, 0, 8, 0, 0, 12, 0, 0},

	    {2, 0, 8, 250, 10, 12, 0, 0},

	    {3, 0, 8, 0, 0, 12, 0, 0},

	    {4, 0, 8, 0, 0, 12, 0, 0},

	    {5, 0, 8, 0, 0, 12, 0, 0},
	};


#endif /*CAPTURE_SCENARIO_H_ */
