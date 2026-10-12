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
#define light4_PORT    PTD
#define light4_PIN     4U
#define light3_PORT    PTD
#define light3_PIN     3U
#define light2_PORT    PTD
#define light2_PIN     2U
#define light1_PORT    PTA
#define light1_PIN     3U
/*! @brief User number of configured pins */
#define NUM_OF_CONFIGURED_PINS0 23
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

