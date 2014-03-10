/** \addtogroup PCA10000
 * @{
 *
 * \file
 * Header file with definitions related to the pins of LED
 * and serial port connections in the PCA10000 platform
 *  \author prithvi
 */

#ifndef BOARD_H
#define BOARD_H
 
/*---------------------------------------------------------------------------*/
/** \name
 * 	LED Pin definitions
 * 	@{
 */
/** 0th bit in Contiki leds.c */
#define LEDS_CONF_GREEN	22
/** 1st bit in Contiki leds.c */
#define LEDS_CONF_BLUE  23
/** 2nd bit in Contiki leds.c */
#define LEDS_CONF_RED   21
/*---------------------------------------------------------------------------*/
/** @} */

/** \name
 * 	UART Pin definitions
 * 	@{
 */
#define RX_PIN_NUMBER  	11
#define TX_PIN_NUMBER  	9
#define CTS_PIN_NUMBER 	10
#define RTS_PIN_NUMBER 	8
/** Hardware flow control required in PCA10000 */
#define HWFC           	true
/*---------------------------------------------------------------------------*/
/** @} */
  
#endif  /* BOARD_H */

/**
 * @}
 */
