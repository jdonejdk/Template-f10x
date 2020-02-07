#include "bsp_led.h"

#define LED_NUM     4

static GPIO_LED leds[LED_NUM] = {
    { RCC_APB2Periph_GPIOF, GPIOF, GPIO_Pin_6 },
    { RCC_APB2Periph_GPIOF, GPIOF, GPIO_Pin_7 },
    { RCC_APB2Periph_GPIOF, GPIOF, GPIO_Pin_8 },
    { RCC_APB2Periph_GPIOF, GPIOF, GPIO_Pin_9 },
};

/*
*********************************************************************************************************
*	函 数 名: bsp_InitLed
*	功能说明: 初始化LED指示灯
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
void bsp_InitLed(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	/* 配置所有的LED指示灯GPIO为推挽输出模式 */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	

    for(uint8_t i = 0; i < LED_NUM; ++i)
    {
        /* 打开GPIOF的时钟 */
        RCC_APB2PeriphClockCmd(leds[i].CLK, ENABLE);
        /* 由于将GPIO设置为输出时，GPIO输出寄存器的值缺省是0，因此会驱动LED点亮
            这是我不希望的，因此在改变GPIO为输出前，先修改输出寄存器的值为1 */
        GPIO_SetBits(leds[i].PORT,  leds[i].PIN);
        
        GPIO_InitStructure.GPIO_Pin = leds[i].PIN;
        GPIO_Init(leds[i].PORT, &GPIO_InitStructure);
    }
}

/*
*********************************************************************************************************
*	函 数 名: bsp_LedOn
*	功能说明: 点亮指定的LED指示灯。
*	形    参：_no : 指示灯序号，范围 0 - ?
*	返 回 值: 按键代码
*********************************************************************************************************
*/
void bsp_LedOn(uint8_t _no)
{
    if(_no < 0 || _no >= LED_NUM) {
        return;
    }
    
	leds[_no].PORT->BRR = leds[_no].PIN;
}

/*
*********************************************************************************************************
*	函 数 名: bsp_LedOff
*	功能说明: 熄灭指定的LED指示灯。
*	形    参：_no : 指示灯序号，范围 0 - LED_NUM-1
*	返 回 值: 按键代码
*********************************************************************************************************
*/
void bsp_LedOff(uint8_t _no)
{
    if(_no < 0 || _no >= LED_NUM) {
        return;
    }
    
    leds[_no].PORT->BSRR = leds[_no].PIN;
}

/*
*********************************************************************************************************
*	函 数 名: bsp_LedToggle
*	功能说明: 翻转指定的LED指示灯。
*	形    参：_no : 指示灯序号，范围 0 - LED_NUM-1
*	返 回 值: 按键代码
*********************************************************************************************************
*/
void bsp_LedToggle(uint8_t _no)
{
    if(_no < 0 || _no > LED_NUM-1) {
        return;
    }
    
    leds[_no].PORT->ODR = leds[_no].PIN;
}
