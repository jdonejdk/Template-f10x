/*
	通信协议：
	(1) 采用ASCII字符通信
	(2) 每个命令帧由$开头，#结束
	(3) PC发往开发板的命令定义：
		$LEDON=1#    			点亮开发板上LED灯, 数字范围：1-4
		$LEDOFF=2#    			熄灭开发板上LED灯, 数字范围：1-4	
		$LEDONALL#    			点亮开发板上所有的LED灯
		$LEDOFFALL#    			熄灭开发板上所有的LED灯
		
	(4) 开发板发往PC的命令定义 (为了便于超级终端换行显示，#后面还加了回车和换行字符\r\n)
		$OK#                    对PC命令的正确应答；如果不正确，则不响应
		$KEY=U#	   				摇杆上键按下
		$KEY=D#					摇杆下键按下
		$KEY=L#					摇杆左键按下
		$KEY=R#					摇杆右键按下
*/

#include "bsp.h"
#include "hw_config.h"			/* USB模块 */

/* 仅允许本文件内调用的函数声明 */
static void InitBoard(void);
static void PrintHelpInfo(void);
static void UsbCmdPro(void);
static void ReportOk(void);
static void AnalyzeCmd(uint8_t *_pCmdBuf, uint16_t _usLen);

/*
*********************************************************************************************************
*	函 数 名: main
*	功能说明: c程序入口
*	形    参：无
*	返 回 值: 错误代码(无需处理)
*********************************************************************************************************
*/
int main(void)
{
	uint8_t ucKeyCode;

	/*
		由于ST固件库的启动文件已经执行了CPU系统时钟的初始化，所以不必再次重复配置系统时钟。
		启动文件配置了CPU主时钟频率、内部Flash访问速度和可选的外部SRAM FSMC初始化。

		系统时钟缺省配置为72MHz，如果需要更改，可以修改：
		\Libraries\CMSIS\CM3\DeviceSupport\ST\STM32F10x\system_stm32f10x.c
		中配置系统时钟的宏。
	*/

	InitBoard();	/* 为了是main函数看起来更简洁些，我们将硬件初始化的代码封装到这个函数 */

	/* 初始化USB设备 */
	bsp_InitUsb();

   	PrintHelpInfo();	/* 打印帮助提示到串口1 */

	while(1)
	{
		CPU_IDLE();

		UsbCmdPro();	/* 处理PC通过USB发来的命令 (非阻塞) */
        usb_SendDataToHost((uint8_t*)"$KEY=U#\r\n", 9);
		ucKeyCode = bsp_GetKey();	/* 读取键值, 无键按下时返回 KEY_NONE = 0 */
		if (ucKeyCode != KEY_NONE)
		{
			/* 有键按下 */
			switch (ucKeyCode)
			{
				case KID_K1:		/* 摇杆UP键按下 */
					usb_SendDataToHost((uint8_t*)"$KEY=U#\r\n", 9);
					break;

				case KID_K2:		/* 摇杆DOWN键按下 */
					usb_SendDataToHost((uint8_t*)"$KEY=D#\r\n", 9);
					break;

				default:
					break;
			}
		}
	}			
}

/*
*********************************************************************************************************
*	函 数 名: PrintHelpInfo
*	功能说明: 将帮助信息打印到串口
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
static void PrintHelpInfo(void)
{
	printf("请安装stm32_vcp USB虚拟串口驱动，然后用串口工具打开这个虚拟串口进行操作。\r\n");
	printf("PC->开发板的命令格式：\r\n");
	printf("  $LEDON=1#     点亮开发板上LED灯, 数字范围：1-4\r\n");
	printf("  $LEDOFF=2#    熄灭开发板上LED灯, 数字范围：1-4\r\n");
	printf("  $LEDONALL#    点亮开发板上所有的LED灯\r\n");
	printf("  $LEDOFFALL#   熄灭开发板上所有的LED灯\r\n");
    
	printf("开发板->PC的汇报格式：\r\n");
	printf("  $OK#          对PC命令的正确应答；如果不正确，则不响应\r\n");
	printf("  $KEY=U#       摇杆上键按下\r\n");
	printf("  $KEY=D#       摇杆下键按\r\n");
	printf("  $KEY=L#       摇杆左键按下\r\n");
	printf("  $KEY=R#       摇杆右键按下\r\n");
}

/*
*********************************************************************************************************
*	函 数 名: UsbCmdPro
*	功能说明: 处理USB口接收到的数据。 非阻塞模式
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
static void UsbCmdPro(void)
{
	uint8_t ucData;
	uint8_t ucNum;
	static uint8_t aCmdBuf[32];
	static uint16_t usPos;
	
	ucData = usb_GetRxByte(&ucNum);	/* 从USB口读取一个字节 ucNum存放读到的字节个数 */
	if (ucNum == 0)
	{
		return;
	}
	usb_SendDataToHost(&ucData, 1);	/* 在PC串口工具回显键入的字符 */
}

/*
*********************************************************************************************************
*	函 数 名: ReportOk
*	功能说明: 相应命令正确应答
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
static void ReportOk(void)
{
	usb_SendDataToHost((uint8_t*)"\r\n$OK#\r\n", 8);;
}

/*
*********************************************************************************************************
*	函 数 名: AnalyzeCmd
*	功能说明: 分析PC机的命令
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
static void AnalyzeCmd(uint8_t *_pCmdBuf, uint16_t _usLen)
{
	/*	
		$LEDON=1#    			点亮开发板上LED灯, 数字范围：1-4
		$LEDOFF=2#    			熄灭开发板上LED灯, 数字范围：1-4	
		$LEDONALL#    			点亮开发板上所有的LED灯
		$LEDOFFALL#    		熄灭开发板上所有的LED灯
		
	开发板发往PC的命令定义
		$OK#                    对PC命令的正确应答；如果不正确，则不响应
		$KEY=U#	   				摇杆上键按下
		$KEY=D#					摇杆下键按下
		$KEY=L#					摇杆左键按下
		$KEY=R#					摇杆右键按下
	*/
	_pCmdBuf[_usLen] = 0;

	if ((_usLen == 7) && (memcmp(_pCmdBuf, "LEDON=", 6) == 0))	  /* 点亮LED灯 */
	{
		ReportOk();	/* 应答OK */
		if ((_pCmdBuf[6] >= '1') && (_pCmdBuf[6] <= '4'))
		{
			bsp_LedOn(_pCmdBuf[6] - '0');
		}
	}
	else if ((_usLen == 8) && (memcmp(_pCmdBuf, "LEDOFF=", 7) == 0))
	{
		ReportOk();	/* 应答OK */
		if ((_pCmdBuf[7] >= '1') && (_pCmdBuf[7] <= '4'))
		{
			bsp_LedOn(_pCmdBuf[7] - '0');
		}
	}
	else if ((_usLen == 8) && (memcmp(_pCmdBuf, "LEDONALL", 8) == 0))
	{
		ReportOk();	/* 应答OK */
		bsp_LedOn(1);
		bsp_LedOn(2);
		bsp_LedOn(3);
		bsp_LedOn(4);
	}
	else if ((_usLen == 9) && (memcmp(_pCmdBuf, "LEDOFFALL", 9) == 0))
	{
		ReportOk();	/* 应答OK */
		bsp_LedOff(1);
		bsp_LedOff(2);
		bsp_LedOff(3);
		bsp_LedOff(4);
	}
	else
	{
		usb_SendDataToHost((uint8_t*)"\r\n$ERRCMD#\r\n", 10);		/* 应答错误 */
	}
}

/*
*********************************************************************************************************
*	函 数 名: InitBoard
*	功能说明: 初始化硬件设备
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
static void InitBoard(void)
{
	/* 配置串口，用于printf输出 */
	bsp_InitUart();

	/* 配置LED指示灯GPIO */
	bsp_InitLed();

	/* 配置按键GPIO, 必须在bsp_InitTimer之前调用 */
	bsp_InitKey();

	/* 初始化systick定时器，并启动定时中断 */
	bsp_InitTimer();
}
