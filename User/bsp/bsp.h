#ifndef _BSP_H_
#define _BSP_H_

/* 定义 BSP 版本号 */
#define __STM32F1_BSP_VERSION		"1.1"

/* CPU空闲时执行的函数 */
#define CPU_IDLE()		bsp_Idle()

/* 开关全局中断的宏 */
#define ENABLE_INT()	__set_PRIMASK(0)	/* 使能全局中断 */
#define DISABLE_INT()	__set_PRIMASK(1)	/* 禁止全局中断 */

/* 这个宏仅用于调试阶段排错 */
#define BSP_Printf		printf
//#define BSP_Printf(...)

#include "stm32f10x.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

//#ifndef TRUE
//	#define TRUE  1
//#endif

//#ifndef FALSE
//	#define FALSE 0
//#endif

#include "bsp_led.h"
#include "bsp_timer.h"
#include "bsp_key.h"

#include "bsp_uart_fifo.h"

/* 提供给其他C文件调用的函数 */
void bsp_Init(void);
void bsp_Idle(void);

#endif
