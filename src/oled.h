#ifndef _OLED_H_
#define _OLED_H_

/*
 * S32K144 + 0.96" SSD1306 OLED，硬件 LPI2C0 驱动。
 *
 * 接线（4 脚模块）：
 *   GND -> GND
 *   VCC -> 3.3V
 *   SCL -> PTA3  (LPI2C0_SCL, 复用 ALT3)
 *   SDA -> PTA2  (LPI2C0_SDA, 复用 ALT3)
 * SCL/SDA 上模块一般自带 4.7k 上拉，若没有请自己补 4.7k 到 3.3V。
 */

#include "sdk_project_config.h"

/* SSD1306 从机 7 位地址 0x3C；有些模块是 0x3D，点不亮就换它 */
#define OLED_I2C_ADDR  0x3CU

void OLED_Init(void);
void OLED_Clear(void);
void OLED_Update(void);                                     /* 把显存整屏刷到 OLED */
void OLED_ShowChar(uint8_t x, uint8_t y, char ch);          /* x: 0~127, y: 0~7(页) */
void OLED_ShowString(uint8_t x, uint8_t y, const char *str);
void OLED_ShowNum(uint8_t x, uint8_t y, int32_t num, uint8_t len);

#endif /* _OLED_H_ */
