#include "main.h"

/*CNT_FLAG 标志当前的程序状态，
0未接收信号，1测量周期中，2测量低电平中
（先测周期可以确保后续低电平准确？）*/
char CNT_FLAG = 1;
char BEGIN_FLAG = 0;   // 标记本次执行是否开始
char FALLING_FLAG = 0; // 记录下降沿触发次数

char buf[30];  // 发送数组
int COUNT = 0; // 标记溢出次数
float DUTY_CYCLE=0; // 占空比

int READ_TIME[3] = 0; // 单位：us

void INTERRUPT_INIT()
{
	TMOD = 0x01; // 定时器0工作在模式1
	TH0 = 0;	 // 定时器初值清零
	TL0 = 0;
	ET0 = 1; // 允许定时器0中断

	IT0 = 1; // 设置外部中断0为下降沿触发
	EX0 = 1; // 允许外部中断0
	EA = 1;	 // 允许总中断
}
void IT_Count() interrupt 1
{
	if (IT0 == 1)
	{
		COUNT++;
	}
}

/*
通过改变外部中断触发方式，测量低电平时长和周期
*/
void ET_0() interrupt 0 // 用于启动定时器和标志程序状态
{
	TR0 = 1; // 启动定时器计数
	if (IT0 == 1)
	{
		FALLING_FLAG++;
		BEGIN_FLAG++;
	}
	if (CNT_FLAG == 2) // 需要标志位标志测量目标
	{
		IT0 = 0; // 设置外部中断0为低电平触发
	}
}
void main()
{
	INTERRUPT_INIT(); // 定时器0+外部中断
	// UartInit();		  // 串口
	Uart1_Init(); // 9600bps@11.0592MHz  12T模式
	while (1)
	{
		if (BEGIN_FLAG == 2 && FALLING_FLAG == 2)
		{
			TR0 = 0;															   // 停止计数
			EA = 0;																   // 开始读数防止中断
			READ_TIME[CNT_FLAG] = (int)((TH0 * 256 + TL0 + COUNT * 65536) * 1.09); // 计算总时间

			sprintf(buf, "T=%d\r\n", READ_TIME[CNT_FLAG]);
			UART_Send(buf); // 发送数据

			COUNT = 0; // 清零计数器
			TH0 = 0;   // 清零定时器
			TL0 = 0;

			IT0 = 1;	  // 设置外部中断0为下降沿触发，确保测量到完整的低电平时间
			CNT_FLAG = 2; // 进入测量低电平状态
			BEGIN_FLAG = 0;
			FALLING_FLAG = 0;
			EA = 1; // 允许中断
		}
		else if (IT0 == 0 && IE0 == 0)
		{
			TR0 = 0;															   // 停止计数
			EA = 0;																   // 开始读数防止中断
			READ_TIME[CNT_FLAG] = (int)((TH0 * 256 + TL0 + COUNT * 65536) * 1.09); // 计算总时间
			sprintf(buf, "TOFF=%d\r\n", READ_TIME[CNT_FLAG]);
			UART_Send(buf); // 发送数据

			DUTY_CYCLE=(float)(READ_TIME[2])/(float)READ_TIME[1];
			sprintf(buf, "D=%f\r\n", DUTY_CYCLE);//此处不要用%d
			UART_Send(buf); // 发送数据

			COUNT = 0; // 清零计数器
			TH0 = 0;   // 清零定时器
			TL0 = 0;

			IT0 = 1;	  // 设置外部中断0为下降沿触发
			CNT_FLAG = 0; // 进入未接收状态
			BEGIN_FLAG = 0;
			FALLING_FLAG = 0;
			EA = 1; // 允许中断
		}
	}
}
/*
问题及解决：
中断标志位清零：除串口，均自动清零
11.21：
考虑解决如何检测低电平时间->下降沿触发后转低电平？
完成串口发送
*/