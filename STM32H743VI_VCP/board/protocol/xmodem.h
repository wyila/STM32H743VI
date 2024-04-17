#ifndef __XMODEM_H
#define __XMODEM_H

#define XMODEM_BUF_SIZE     128
#define YMODEM_BUF_SIZE     1024

//xmodem 128字节头标志
#define SOH     0x01
//xmodem 1024字节头标志
#define STX     0x02
//结束
#define EOT     0x04
//数据正确
#define ACK     0x06
//数据有误
#define NAK     0x15
//无条件结束
#define CAN     0x18
//以CRC16校验的方式传输数据
#define CRC16   0x43

uint16_t CRC16_Verify(const uint8_t *in_buf, uint16_t len);

#endif
