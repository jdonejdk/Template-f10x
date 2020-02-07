#ifndef __BSP_LED_H
#define __BSP_LED_H

#include "bsp.h"

typedef struct gpio_led
{
    uint32_t    CLK;
 	GPIO_TypeDef* PORT;
	uint32_t    PIN;
} GPIO_LED;

/* 供外部调用的函数声明 */
void bsp_InitLed(void);
void bsp_LedOn(uint8_t _no);
void bsp_LedOff(uint8_t _no);
void bsp_LedToggle(uint8_t _no);

#endif

