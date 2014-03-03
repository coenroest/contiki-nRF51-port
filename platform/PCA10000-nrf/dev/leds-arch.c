/*
 * leds-arch.c
 *
 *  Created on: 03-Feb-2014
 *      Author: prithvi
 */

#include "leds.h"
#include "nrf_gpio.h"
#include "board.h"

/**@brief Function to initialize the LEDS in the PCA10000 board
 *
 */
void leds_arch_init(void){
	  nrf_gpio_cfg_output(LEDS_CONF_RED);
	  nrf_gpio_pin_set(LEDS_CONF_RED);
	  nrf_gpio_cfg_output(LEDS_CONF_GREEN);
	  nrf_gpio_pin_set(LEDS_CONF_GREEN);
	  nrf_gpio_cfg_output(LEDS_CONF_BLUE);
	  nrf_gpio_pin_set(LEDS_CONF_BLUE);
}
unsigned char leds_arch_get(void){
	unsigned char temp = (nrf_gpio_pin_read(LEDS_CONF_RED)>>2)|(nrf_gpio_pin_read(LEDS_CONF_BLUE)>>1)|nrf_gpio_pin_read(LEDS_CONF_GREEN);
	return ((~temp)&0x07);
}
void leds_arch_set(unsigned char leds){
	nrf_gpio_pin_write(LEDS_CONF_GREEN,(~leds)&0x01);
	nrf_gpio_pin_write(LEDS_CONF_BLUE,(~leds)&0x02);
	nrf_gpio_pin_write(LEDS_CONF_RED,(~leds)&0x04);
}
