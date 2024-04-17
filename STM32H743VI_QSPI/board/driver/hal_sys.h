#ifndef __HAL_SYS_H
#define __HAL_SYS_H

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "stm32h7xx_hal.h"

typedef uint32_t  u32;
typedef uint16_t u16;
typedef uint8_t  u8;

#define SDRAM_ADDR  0xD0000000
#define QSPI_ADDR   0x90000000


extern volatile unsigned int SysTime;

void delay_init(void);
void delay_us(unsigned int us);
void delay_ms(unsigned int ms);
void Error_Handler(void);
void SystemClock_Config(void);
void MPU_Config(void);
int string_to_number(uint8_t *buf, uint8_t len);


//以下为M7内核专用
void CPU_CACHE_Enable(void);
void CM7_Core_Init(void);


#if defined(__clang__) //使用V6编译器(clang)
void __attribute__((noinline)) WFI_SET(void);
void __attribute__((noinline)) INTX_DISABLE(void);
void __attribute__((noinline)) INTX_ENABLE(void);
void __attribute__((noinline)) MSR_MSP(u32 addr);
#elif defined (__CC_ARM)    //使用V5编译器(ARMCC)
//以下为汇编函数
void WFI_SET(void);		//执行WFI指令
void INTX_DISABLE(void);//关闭所有中断
void INTX_ENABLE(void);	//开启所有中断
void MSR_MSP(u32 addr);	//设置堆栈地址
#endif



#endif

