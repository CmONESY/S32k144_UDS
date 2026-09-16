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
void init(void)
{
	CLOCK_SYS_Init(g_clockManConfigsArr,CLOCK_MANAGER_CONFIG_CNT,
			g_clockManCallbacksArr,CLOCK_MANAGER_CALLBACK_CNT);
	CLOCK_SYS_UpdateConfiguration(0,CLOCK_MANAGER_POLICY_AGREEMENT);
	PINS_DRV_Init(NUM_OF_CONFIGURED_PINS0, g_pin_mux_InitConfigArr0);
}

void lpit0(void)
{
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
void pwm_init(void)
{
	FTM_DRV_Init(INST_FLEXTIMER_PWM_1,&flexTimer_pwm_1_InitConfig, &ftmstate);
	FTM_DRV_InitPwm(INST_FLEXTIMER_PWM_1, &flexTimer_pwm_1_PwmConfig);
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
