#include "main.h"

void ms_delay(u16 t)
{
	unsigned char data i, j;

	while (t--)
	{

		_nop_();
		i = 2;
		j = 199;
		do
		{
			while (--j)
				;
		} while (--i);
	}
}

void Uart1_Init(void) // 9600bps@11.0592MHz  12T模式
{
	SCON = 0x50;  // 8位数据,可变波特率,REN置位允许串行接收状态
	PCON |= 0x80; // SMOD波特率选择位为1，波特率加倍

	TMOD &= 0x0F; // 设置定时器模式，8位定时器，自动重装，波特率比16位准确
	TMOD |= 0x20;

	TL1 = 0xFA; // 设置定时初始值
	TH1 = TL1;	// 设置定时重载值

	ET1 = 0; // 禁止定时器中断
	TR1 = 1; // 定时器1开始计时
	ES = 0;	 // 打开串口中断允许位
	EA = 1;	 // 打开总中断
}
void UartInit(void) // 9600bps@12.000MHz
{
	PCON |= 0x80; // 使能波特率倍速位SMOD
	SCON = 0x50;  // 8位数据,可变波特率
	// AUXR &= 0xBF;		//定时器1时钟为Fosc/12,即12T
	// AUXR &= 0xFE;		//串口1选择定时器1为波特率发生器
	TMOD &= 0x0F; // 清除定时器1模式位
	TMOD |= 0x20; // 设定定时器1为8位自动重装方式
	TL1 = 0xF9;	  // 设定定时初值
	TH1 = 0xF9;	  // 设定定时器重装值
	ET1 = 0;	  // 禁止定时器1中断
	TR1 = 1;	  // 启动定时器1
	ES = 0;		  // 打开串口中断允许位
	EA = 1;		  // 打开总中断
}

void UART_SendByte(u8 Byte)
{
	ES = 0; // 关闭串口中断
	TI = 0; // 清发送完毕中断请求标志位
	SBUF = Byte;
	while (TI == 0)
		; // 发送完成置1，需要软件复位
	TI = 0;
	ES = 1; // 允许串口中断
}

void UART_Send12(u8 *str)
{
	while (*str != '\0')
	{
		UART_SendByte(*str);
		str++;
	}
}

void UART_Send(u8 *str)
{
	while (*str != '\0')
	{
		UART_SendByte(*str);
		str++;
	}
	UART_SendByte('\r'); // 回车
	UART_SendByte('\n'); // 换行
}
