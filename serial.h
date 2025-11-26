#ifndef __SERIAL_H__
#define __SERIAL_H__

void Uart1_Init(void); // 9600bps@11.0592MHz  12T模式
void UartInit(void);   // 9600bps@12.000MHz

void UART_Send(u8 *str);

#endif // !__SERIAL_H__