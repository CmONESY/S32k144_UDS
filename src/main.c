/*!
** Copyright 2020 NXP
** @file main.c
** @brief
**         Main module.
**         This module contains user's application code.
*/
/*!
**  @addtogroup main_module main module documentation
**  @{
*/
/* MODULE main */


/* Including necessary configuration files. */
volatile int exit_code = 0;
/* User includes */
#include <sdk_project_config.h>
#include "test.h"
/*!
  \brief The main function forW the project.
  \details The startup initialization sequence is the following:
 * - startup asm routine
 * - main()
*/
int main(void)
{
    /* Write your code here */
	init();
	lpit_init();
	lpuart_init();
	pwm_init();
	PINS_DRV_WritePin(LED_YELLOW_PORT,LED_YELLOW_PIN,1);
	PINS_DRV_WritePin(LED_BLUE_PORT,LED_BLUE_PIN,1);
	PINS_DRV_WritePin(LED_RED_PORT,LED_RED_PIN,1);
	PINS_DRV_WritePin(LED_GREEN_PORT,LED_GREEN_PIN,1);
	for(;;)
    {
		//send();
		sg90();
//		if((PINS_DRV_ReadPins(KEY1_PORT) & (1U << KEY1_PIN)) == 0U)
//		{
//			PINS_DRV_WritePin(LED_BLUE_PORT,LED_BLUE_PIN,0);
//
//		}
//		else
//		{
//			PINS_DRV_WritePin(LED_BLUE_PORT,LED_BLUE_PIN,1);
//		}
//		if((PINS_DRV_ReadPins(KEY2_PORT) & (1U << KEY2_PIN)) == 0U)
//				{
//					PINS_DRV_WritePin(LED_GREEN_PORT,LED_GREEN_PIN,0);
//				}
//				else
//				{
//					PINS_DRV_WritePin(LED_GREEN_PORT,LED_GREEN_PIN,1);
//				}
//		/*if((PINS_DRV_ReadPins(KEY3_PORT) & (1U << KEY3_PIN)) == 0U)
//				{
//					PINS_DRV_WritePin(LED_RED_PORT,LED_RED_PIN,0);
//				}
//				else
//				{
//					PINS_DRV_WritePin(LED_RED_PORT,LED_RED_PIN,1);
//				}*/
//		if((PINS_DRV_ReadPins(KEY4_PORT) & (1U << KEY4_PIN)) == 0U)
//				{
//					PINS_DRV_WritePin(LED_YELLOW_PORT, LED_YELLOW_PIN,0);
//				}
//				else
//				{
//					PINS_DRV_WritePin(LED_YELLOW_PORT, LED_YELLOW_PIN,1);
//				}
//
        if(exit_code != 0)
        {
            break;
        }
    }
    return exit_code;
}

/* END main */
/*!
** @}
*/
