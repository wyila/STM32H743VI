#include <stdint.h>
#include "xmodem.h"


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


