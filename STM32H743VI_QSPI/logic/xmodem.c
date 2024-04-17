#include <stdint.h>
#include "xmodem.h"
#include "hal_sys.h"
#include "usart.h"


//可直接复制粘贴到上位机
uint16_t CRC16_Verify(const uint8_t *in_buf, uint16_t len)
{
    uint8_t i;
    uint16_t crc16 = 0;

    while(len --)
    {
        crc16 = crc16 ^ ((unsigned short)(* (in_buf ++)) << 8);
        i = 8;
        while(i --)
        {
            if(crc16 & 0x8000)
                crc16 = (crc16 << 1) ^ 0x1021;
            else
                crc16 <<= 1;
        }
    }
    return crc16;
}


void Xmodem_Send(uint8_t *buf, uint32_t len)
{
    USART1_Send(buf, len);
}

__weak void Xmodem_Copy_Data(uint8_t *buf, uint32_t len)
{
    
}


int Xmodem_Handshake(void)
{
    uint32_t tmp, timeout = SysTime;
    
    while(1)
    {
        if((SysTime - timeout) > 60000)//一分钟握手失败退出
            return 1;
        if(tmp != SysTime)//一秒发送一次握手
        {
            tmp = SysTime;
            if((tmp % 1000) == 0)
                printf("C");
        }
        if(USART1_RX_MQ_PI->state)//有数据后退出
            break;
    }
    return 0;
}


int Xmodem_Get_Data(void)
{
    uint32_t timeout = SysTime;
    uint32_t len;
    uint16_t CRC16_VAL;
    uint8_t *buf;
    uint8_t ret = 0;
    
    while(1)
    {
        if(USART1_RX_MQ_PI->state)
        {
            USART1_RX_MQ_PI->state = 0;
            len = USART1_RX_MQ_PI->len;
            buf = USART1_RX_MQ_PI->data;
            USART1_RX_MQ_PI = USART1_RX_MQ_PI->next;
            
            if(len == 1)//上位机发送停止指令
            {
                switch(buf[0])
                {
                    case EOT:
                    case CAN:
                        ret = ACK;
                        Xmodem_Send(&ret, 1);
                        return 0;
                }
            }
            else//正常接收数据
            {
                if(len < 6)//数据太短，不符合协议要求
                {
                    ret = NAK;
                    Xmodem_Send(&ret, 1);
                    continue;
                }//接受不定长数据
                CRC16_VAL = CRC16_Verify(&buf[3], len - 5);
                if(CRC16_VAL != *(uint16_t*)(&buf[len - 2]))//数据校验
                    ret = NAK;
                else
                {
                    ret = ACK;//校验成功
                    Xmodem_Copy_Data(&buf[3], len - 5);//数据转移
                }
                Xmodem_Send(&ret, 1);//回复上位机
            }
            timeout = SysTime;
        }
        if((SysTime - timeout) > 6000)//6S无数据进入退出程序
            return 1;
    }
}


void Xmodem_Transfer(void)
{
    Xmodem_Handshake();
    Xmodem_Get_Data();
}






