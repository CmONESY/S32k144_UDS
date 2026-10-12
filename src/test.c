#include "test.h"
#include "string.h"
#include "interrupt_manager.h"
#include <sdk_project_config.h>
unsigned char rec_buf1[5];
unsigned char rec_buf2[5];
unsigned char *send1="sssss";
unsigned char *send2="12345";
unsigned char *send3="LPUAER0 Recepted! \n";
unsigned char *send4="LPUART1 Recepted! \n";
volatile uint8_t ready_to_send1=0;
volatile uint8_t ready_to_send2=0;
volatile uint8_t led_sel=0,dir=0;
ftm_state_t ftmstate;
volatile uint16_t count=0;
volatile status_t pwm_update_status=STATUS_SUCCESS;
#define LIGHT_NUM   5U      /* light1 ~ light5 */
#define LIGHT_ON    1U      /* 低电平点亮；如果是高电平点亮，把这两个值对调 */
#define LIGHT_OFF   0U

/* 只点亮 idx 对应的那一盏，其余全灭 */
static void light_show(uint8_t idx)
{
	static GPIO_Type * const light_port[LIGHT_NUM] = {light1_PORT, light2_PORT, light3_PORT, light4_PORT, light5_PORT};
	static const uint16_t   light_pin[LIGHT_NUM]   = {light1_PIN,  light2_PIN,  light3_PIN,  light4_PIN,  light5_PIN};
	uint8_t i;

	for(i = 0U; i < LIGHT_NUM; i++)
	{
		PINS_DRV_WritePin(light_port[i], light_pin[i], (i == idx) ? LIGHT_ON : LIGHT_OFF);
	}
}

void led(void)
{
	static uint8_t light_idx = LIGHT_NUM;   /* LIGHT_NUM 表示还没点灯，第一次按下从 light1 开始 */
	static uint8_t key_last  = 1U;          /* 上一次按键电平，1 = 松开 */
	uint8_t key_now = ((PINS_DRV_ReadPins(KEY5_PORT) & (1U << KEY5_PIN)) != 0U) ? 1U : 0U;

	if((key_last != 0U) && (key_now == 0U))  /* 下降沿：按一下 */
	{
		light_idx = (light_idx >= (LIGHT_NUM - 1U)) ? 0U : (uint8_t)(light_idx + 1U);  /* 最后再回到 light1 */
		light_show(light_idx);
	}

	key_last = key_now;
}
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
//	LPUART_DRV_SendData(INST_LPUART_1,(const uint8_t *)send1, strlen(send1));
//	LPUART_DRV_SendData(INST_LPUART_2,(const uint8_t *)send2, strlen(send2));

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
	flexTimer_pwm_1_InitConfig.syncMethod.syncPoint=FTM_UPDATE_NOW;
	FTM_DRV_Init(INST_FLEXTIMER_PWM_1,&flexTimer_pwm_1_InitConfig, &ftmstate);
	FTM_DRV_InitPwm(INST_FLEXTIMER_PWM_1, &flexTimer_pwm_1_PwmConfig);
}
status_t sweep(bool direction)
{
	if(direction)
	{
		pwm_update_status=FTM_DRV_UpdatePwmChannel(INST_FLEXTIMER_PWM_1, flexTimer_pwm_1_IndependentChannelsConfig[0].hwChannelId
				,FTM_PWM_UPDATE_IN_DUTY_CYCLE, 3277U, 0U, true);
	}
	else
	{
		pwm_update_status=FTM_DRV_UpdatePwmChannel(INST_FLEXTIMER_PWM_1, flexTimer_pwm_1_IndependentChannelsConfig[0].hwChannelId
				,FTM_PWM_UPDATE_IN_DUTY_CYCLE, 1638U, 0U, true);
	}
	PINS_DRV_WritePin(LED_RED_PORT, LED_RED_PIN, pwm_update_status == STATUS_SUCCESS ? direction : 1U);
	return pwm_update_status;
}
void sg90(void)
{
	if(PINS_DRV_ReadPins(KEY5_PORT)&(1<<KEY5_PIN))
	{
		sweep(1);
	}
	else sweep(0);
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
