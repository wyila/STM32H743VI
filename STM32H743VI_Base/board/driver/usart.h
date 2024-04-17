#ifndef __USART_H
#define __USART_H

#define PRINT_USART USART3

#define USART_FIFO_SIZE     200
#define USART_BUF_SIZE      2000

#define UART_BUF_SIZE   USART_BUF_SIZE

#include "stm32h7xx_hal.h"
#include "message_queue.h"

#define USART_IT_PE         0x0001  /* 奇偶校验错误 */
#define USART_IT_FE         0x0002  /* 帧错误 */
#define USART_IT_NF         0x0004  /* START位噪声检测 */
#define USART_IT_ORE        0x0008  /* 溢出错误 */
#define USART_IT_IDLE       0x0010  /* 空闲中断 */
#define USART_IT_RXNE       0x0020  /* 数据寄存器不为空 */
#define USART_IT_TC         0x0040  /* 发送完成 */
#define USART_IT_TXE        0x0080  /* 发送数据寄存器为空 */
#define USART_IT_LBD        0x0100  /* LIN检测（同步模式下使用，一般用不到） */
#define USART_IT_CTS        0x0400  /* CTS标志，应该是RS232标准才用到 */

extern UART_HandleTypeDef USART1_HAND;
extern UART_RX_QUEUE *USART1_RX_MQ_PI;

void USART1_Init(uint32_t Baud_rate);
uint8_t USART1_Send(uint8_t *buf, uint32_t len);
void USART_DeInit(void);

#endif








