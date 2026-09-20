#include "test.h"
#include "string.h"
#include "interrupt_manager.h"
unsigned char rec_buf1[5];
unsigned char rec_buf2[5];
unsigned char *send1="sssss";
unsigned char *send2="12345";
unsigned char *send3="LPUAER0 Recepted! \n";
unsigned char *send4="LPUART1 Recepted! \n";
volatile uint8_t ready_to_send1=0;
volatile uint8_t ready_to_send2=0;
ftm_state_t ftmstate;
int count=0;
void init(void)
{
	CLOCK_SYS_Init(g_clockManConfigsArr,CLOCK_MANAGER_CONFIG_CNT,
			g_clockManCallbacksArr,CLOCK_MANAGER_CALLBACK_CNT);
	CLOCK_SYS_UpdateConfiguration(0,CLOCK_MANAGER_POLICY_AGREEMENT);
	PINS_DRV_Init(NUM_OF_CONFIGURED_PINS0, g_pin_mux_InitConfigArr0);
}

void lpit0(void)
{
	count++;
	LPIT_DRV_ClearInterruptFlagTimerChannels(INST_LPIT_CONFIG_1, 0X01U);
	PINS_DRV_TogglePins(LED_YELLOW_PORT,1<<LED_YELLOW_PIN);
	LPUART_DRV_SendData(INST_LPUART_1,(const uint8_t *)send1, strlen(send1));
	LPUART_DRV_SendData(INST_LPUART_2,(const uint8_t *)send2, strlen(send2));
}
void lpit_init(void)
{
	LPIT_DRV_Init(INST_LPIT_CONFIG_1, &lpit1_InitConfig);
	INT_SYS_InstallHandler(LPIT0_Ch0_IRQn, &lpit0, (isr_t*)0U);
	LPIT_DRV_InitChannel(INST_LPIT_CONFIG_1, 0, &lpit1_ChnConfig0);
	LPIT_DRV_StartTimerChannels(INST_LPIT_CONFIG_1, 0X01U);
}
void send()
{
	if(ready_to_send1)
	{
		LPUART_DRV_SendData(INST_LPUART_2,(const uint8_t *)send3,strlen(send3));
		ready_to_send1=0u;
	}
	if(ready_to_send2)
	{
		LPUART_DRV_SendData(INST_LPUART_1,(const uint8_t *)send4,strlen(send4));
		ready_to_send2=0u;
	}

}
void uart_receive1(void *drivestate,uart_event_t event,void *userData)
{
	if(event ==  UART_EVENT_RX_FULL)
	{
		PINS_DRV_TogglePins(LED_GREEN_PORT,1<<LED_GREEN_PIN);
		ready_to_send1=1;
		LPUART_DRV_ReceiveData(INST_LPUART_1, rec_buf1, 5u);
		*rec_buf1=" ";
	}
	if(event == UART_EVENT_END_TRANSFER || event == UART_EVENT_ERROR)
	{
		LPUART_DRV_ReceiveData(INST_LPUART_1, rec_buf1, 5u);
	}
}
void uart_receive2(void *drivestate,uart_event_t event,void *userData)
{
	if(event ==  UART_EVENT_RX_FULL)
	{
		PINS_DRV_TogglePins(LED_BLUE_PORT,1<<LED_BLUE_PIN);
		ready_to_send2=1;
		LPUART_DRV_ReceiveData(INST_LPUART_2, rec_buf2, 5u);
		*rec_buf2=" ";
	}
	if(event == UART_EVENT_END_TRANSFER || event == UART_EVENT_ERROR)
	{
		LPUART_DRV_ReceiveData(INST_LPUART_2, rec_buf2, 5u);
	}
}
void lpuart_init(void)
{
	LPUART_DRV_Init(INST_LPUART_1, &lpUartState1, &lpuart_1_InitConfig0);
	LPUART_DRV_InstallRxCallback(INST_LPUART_1, &uart_receive1, NULL);
	LPUART_DRV_ReceiveData(INST_LPUART_1, rec_buf1, 5u);

	LPUART_DRV_Init(INST_LPUART_2, &lpUartState0, &lpuart_0_InitConfig0);
	LPUART_DRV_InstallRxCallback(INST_LPUART_2, &uart_receive2, NULL);
	LPUART_DRV_ReceiveData(INST_LPUART_2, rec_buf2, 5u);
}
/*================ SG90 servo on FTM0_CH1 / PTB13, 50Hz, 20ms frame ================*/
/*
 * NOTE: FTM_DRV_GetFrequency() uses CORE_CLK (80MHz) for
 * FTM_CLOCK_SOURCE_SYSTEMCLK, so FTM_DRV_InitPwm() writes MOD=49999, i.e. it
 * assumes 50000 counts equal 20ms. But the Clocks config gives FTM0_CLK = 8MHz
 * (SIRC_DIV1), /32 = 250kHz. If the real counter clock is 250kHz, MOD=49999
 * means a 200ms (5Hz) frame, the servo only slams to one end at power up and
 * then never moves again. Here the period is re-programmed to a real 20ms and
 * the pulse width is converted with 250kHz (1 count = 4us).
 *
 * If the servo instead buzzes/jitters at high frequency after flashing, the real
 * counter clock is 2.5MHz, then change:
 *   FTM_TICKS_PER_US_NUM/DEN -> 5U/2U  and  FTM_PERIOD_TICKS -> 50000U
 */
#define FTM_TICKS_PER_US_NUM    1U
#define FTM_TICKS_PER_US_DEN    4U
#define FTM_PERIOD_TICKS        5000U    /* 250kHz * 20ms = 5000 counts */

/* 0.5/2.5ms often drive the horn into the mechanical stop and the servo keeps
 * buzzing there. 0.8/2.2ms is inside the travel of most SG90s. */
#define SG90_MIN_US             800U     /* 0.8ms */
#define SG90_MID_US             1500U    /* 1.5ms center */
#define SG90_MAX_US             2200U    /* 2.2ms */

static uint16_t servo_us_to_ticks(uint16_t us)
{
	return (uint16_t)(((uint32_t)us * FTM_TICKS_PER_US_NUM) / FTM_TICKS_PER_US_DEN);
}

void servo_set_us(uint16_t us)
{
	(void)FTM_DRV_UpdatePwmChannel(INST_FLEXTIMER_PWM_1,
								(uint8_t)flexTimer_pwm_1_IndependentChannelsConfig[0].hwChannelId,
								FTM_PWM_UPDATE_IN_TICKS,
								servo_us_to_ticks(us), 0U, true);
}

void pwm_init(void)
{
	(void)FTM_DRV_Init(INST_FLEXTIMER_PWM_1,&flexTimer_pwm_1_InitConfig, &ftmstate);
	(void)FTM_DRV_InitPwm(INST_FLEXTIMER_PWM_1, &flexTimer_pwm_1_PwmConfig);
	/* re-program the period to a real 20ms (50Hz) */
	(void)FTM_DRV_UpdatePwmPeriod(INST_FLEXTIMER_PWM_1, FTM_PWM_UPDATE_IN_TICKS, FTM_PERIOD_TICKS, true);
	/* move the servo to its center position first */
	servo_set_us(SG90_MID_US);
}
void sweep(bool direction)
{
	if(direction)
	{
		PINS_DRV_WritePin(LED_GREEN_PORT,LED_GREEN_PIN,1);
		servo_set_us(SG90_MIN_US);
	}
	else
	{
		PINS_DRV_WritePin(LED_GREEN_PORT,LED_GREEN_PIN,0);
		servo_set_us(SG90_MAX_US);
	}
}
void sg90(void)
{
	/*
	 * The generated FTM configuration uses ftmSyncPoint = FTM_UPDATE_NOW, which
	 * maps to SYNCONF[SWRSTCNT] = 1: every software trigger issued by
	 * FTM_DRV_UpdatePwmChannel(..., true) reloads the FTM counter with CNTIN.
	 * Calling it on every pass of the main loop (as this function used to) resets
	 * the counter every few microseconds, so the counter never reaches CnV and
	 * the 20ms frame is never produced - the pin just sits at a static level.
	 * Write the new pulse width only when the target actually changes.
	 */
	static uint8_t last_state = 0xFFU;   /* 0xFF = nothing written yet */
	uint8_t state = ((PINS_DRV_ReadPins(KEY5_PORT) & (1U << KEY5_PIN)) != 0U) ? 1U : 0U;

	if (state != last_state)
	{
		last_state = state;
		sweep(state != 0U);
	}
}

















//void lpit0(void)
//{
//	LPIT_DRV_ClearInterruptFlagTimerChannels(INST_LPIT_CONFIG_1, 0U);
//	PINS_DRV_TogglePins(LED_RED_PORT, (1U << LED_RED_PIN));
//}
//void lpit_init(void)
//{
//	LPIT_DRV_Init(INST_LPIT_CONFIG_1, &lpit1_InitConfig);
//	INT_SYS_InstallHandler(LPIT0_Ch0_IRQn,&lpit0,(isr_t *)0U);
//	LPIT_DRV_InitChannel(INST_LPIT_CONFIG_1, 0, &lpit1_ChnConfig0);
//	LPIT_DRV_StartTimerChannels(INST_LPIT_CONFIG_1, 0X01U);
//}
