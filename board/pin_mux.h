#ifndef _PIN_MUX_H_
#define _PIN_MUX_H_

#include "pins_driver.h"

/***********************************************************************************************************************
 * Definitions
 **********************************************************************************************************************/

/*!
 * @addtogroup pin_mux
 * @{
 */

/***********************************************************************************************************************
 * API
 **********************************************************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif


/*! @brief Definitions/Declarations for BOARD_InitPins Functional Group */
/*! @brief User definition pins */
#define light5_PORT    PTA
#define light5_PIN     0U
#define light3_PORT    PTD
#define light3_PIN     4U
#define light1_PORT    PTD
#define light1_PIN     3U
#define light2_PORT    PTD
#define light2_PIN     2U
#define scl_PORT    PTA
#define scl_PIN     3U
#define sda_PORT    PTA
#define sda_PIN     2U
#define light4_PORT    PTB
#define light4_PIN     12U
/*! @brief User number of configured pins */
#define NUM_OF_CONFIGURED_PINS0 25
/*! @brief User configuration structure */
extern pin_settings_config_t g_pin_mux_InitConfigArr0[NUM_OF_CONFIGURED_PINS0];


#if defined(__cplusplus)
}
#endif

/*!
 * @}
 */
#endif /* _PIN_MUX_H_ */

/***********************************************************************************************************************
 * EOF
 **********************************************************************************************************************/

