#ifndef _TEST_H_
#define _TEST_H
#include "sdk_project_config.h"
#define KEY1_PORT PTC
#define KEY2_PORT PTC
#define KEY3_PORT PTB
#define KEY4_PORT PTB
#define KEY5_PORT PTC

#define KEY1_PIN 12U
#define KEY2_PIN 13U
#define KEY3_PIN 2U
#define KEY4_PIN 3U
#define KEY5_PIN 14U

#define LED_BLUE_PORT PTD
#define LED_GREEN_PORT PTD
#define LED_RED_PORT PTD
#define LED_YELLOW_PORT PTD

#define LED_BLUE_PIN 16U
#define LED_GREEN_PIN 15U
#define LED_RED_PIN 1U
#define LED_YELLOW_PIN 0U
#define Pluse_forward 3000U
#define Pluse_backward 2000U
void init(void);
void lpit0(void);
void lpit_init(void);
extern unsigned char rec_buf[5][10];
void uart_receive(void *drivestate,uart_event_t event,void *userData);
void lpuart_init(void);
void pwm_init(void);
void servo_set_us(uint16_t us);
void sweep(bool direction);
void sg90(void);
extern int count;
#endif
