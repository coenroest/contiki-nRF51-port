/**
 * \addtogroup nrf51-init
 * @{
 * \file
 * nrf51822 initializations
 *  \author CoenRoest
 */

#include "nrf-init.h"
#include "contiki-conf.h"
#include "nrf-gpio.h"
#include "nrf-gpiote.h"
#include "board.h"

#ifndef GPIO_OUTPUT_PIN_NUMBER
#define GPIO_OUTPUT_PIN_NUMBER LEDS_CONF_GREEN  /**< Pin number for LED toggle */
#endif
#define GPIOTE_CHANNEL_NUMBER  0      		/**< GPIOTE channel number. */

void
nrf_init (void)
{
  gpiote_init ();
  ppi_init ();

}

/** @brief Function for initializing Programmable Peripheral Interconnect (PPI) peripheral.
 *   The PPI is needed to convert the radio event into a GPIO task.
 */
void
ppi_init (void)
{

  /* Configure PPI channel 0 to capture the time at which a RADIO BCMATCH event happens */
  NRF_PPI->CH[0].EEP = (uint32_t) &NRF_RADIO->EVENTS_BCMATCH;
  NRF_PPI->CH[0].TEP = (uint32_t) &NRF_TIMER0->TASKS_CAPTURE[BCC_REG];

  /* Configure PPI channel 1 to toggle GPIO_OUTPUT_PIN on every the RADIO ADDRESS event */
  NRF_PPI->CH[1].EEP = (uint32_t) &NRF_RADIO->EVENTS_ADDRESS;
  NRF_PPI->CH[1].TEP = (uint32_t) &NRF_GPIOTE->TASKS_OUT[GPIOTE_CHANNEL_NUMBER];

  /* Configure PPI channel 2 to toggle GPIO_OUTPUT_PIN on every the RADIO END event */
  NRF_PPI->CH[2].EEP = (uint32_t) &NRF_RADIO->EVENTS_END;
  NRF_PPI->CH[2].TEP = (uint32_t) &NRF_GPIOTE->TASKS_OUT[GPIOTE_CHANNEL_NUMBER];


  /* Enable PPI channel 0 (BCcounter), channel 1 (GPIO pin), channel 2 (GPIO pin) and channel 26 (timestamp address event) */
  NRF_PPI->CHEN = (PPI_CHEN_CH0_Enabled << PPI_CHEN_CH0_Pos) |
		  (PPI_CHEN_CH1_Enabled << PPI_CHEN_CH1_Pos) |
		  (PPI_CHEN_CH2_Enabled << PPI_CHEN_CH2_Pos) |
		  (PPI_CHEN_CH26_Enabled << PPI_CHEN_CH26_Pos);
}

void
gpiote_init (void)
{
  // Configure GPIO_OUTPUT_PIN_NUMBER as an output.
  nrf_gpio_cfg_output (GPIO_OUTPUT_PIN_NUMBER);

  // Configure GPIOTE_CHANNEL_NUMBER to toggle the GPIO pin state with input.
  // @note Only one GPIOTE task can be coupled to an output pin.
  nrf_gpiote_task_config (GPIOTE_CHANNEL_NUMBER, GPIO_OUTPUT_PIN_NUMBER,
			  NRF_GPIOTE_POLARITY_TOGGLE,
			  NRF_GPIOTE_INITIAL_VALUE_LOW);
}

/**
 * @}
 */
