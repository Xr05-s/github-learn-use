#ifndef _MAIN_H_
#define _MAIN_H_

#define u8 unsigned char
#define u16 unsigned int

#include <REGX52.H>
#include <stdio.h>
#include "serial.h"
#include <intrins.h>

// main
/*头文件声明附加extern*/
extern char buf[30];  // 发送数组
extern int COUNT; // 标记溢出次数|此处不可赋值 = 0

#endif