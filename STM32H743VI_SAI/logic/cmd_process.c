#include "hal_sys.h"
#include "adc.h"
#include "iic_master.h"
#include "cmd_process.h"



void IIC_Write_Process(struct IIC_GPIO *IIC, uint8_t *buf, uint32_t len)
{
    uint8_t iic_addr, reg_addr, cnt = 0;
    uint8_t send_buf[10];
    uint32_t size;

    // iic_write 0xVV 0xYY 0xZZ;
    // iic_write xx xx x
    if(len < 17)
        return ;

    for(size = 0; size < len; size ++)
    {
        if(buf[size] == ' ')
            buf[size] = 0;
    }//len = 17

    size = strlen((char *)buf) + 1;//size = 9 + 1
    memcpy(buf, &buf[size], len - size);//buf = "xx xx x"
    len -= size;//len = 17 - 10;

    size = strlen((char *)buf) + 1;//size = 2 + 1
    iic_addr = string_to_number(buf, size);
    memcpy(buf, &buf[size], len - size); //buf = "xx x"
    len -= size;//len = 7 - 3;

    size = strlen((char *)buf) + 1;//size = 2 + 1
    reg_addr = string_to_number(buf, size);
    memcpy(buf, &buf[size], len - size); //buf = "x"
    len -= size;//len = 4 - 3;
    buf[len] = 0;

    while(1)
    {
        size = strlen((char *)buf) + 1;
        send_buf[cnt ++] = string_to_number(buf, size);
        if(size >= len)
            break;
        len -= size;
        memcpy(buf, &buf[size], len);
        buf[len] = 0;
    }
    IIC_Write(IIC, iic_addr, reg_addr, send_buf, cnt);
}


void IIC_Read_Process(struct IIC_GPIO *IIC, uint8_t *buf, uint32_t len)
{
    uint8_t iic_addr, reg_addr;
    uint16_t i, size = len;
    // iic_read xxxx xxxx xx;
    // iic_read xx xx x;
    if(len < 16)//16
        return ;

    for(size = 0; size < len; size ++)
    {
        if(buf[size] == ' ')
            buf[size] = 0;
    }

    //len = 16
    size = strlen((char *)buf) + 1;//size = 8 + 1
    memcpy(buf, &buf[size], len - size);//buf = "xx xx x"
    len -= size;//len = 16 - 9;

    size = strlen((char *)buf) + 1;//size = 2 + 1
    iic_addr = string_to_number(buf, size);
    memcpy(buf, &buf[size], len - size); //buf = "xx x"
    len -= size;//len = 7 - 3;

    size = strlen((char *)buf) + 1;//size = 2 + 1
    reg_addr = string_to_number(buf, size);
    memcpy(buf, &buf[size], len - size); //buf = "x"
    len -= size;//len = 4 - 3;
    buf[len] = 0;

    size = string_to_number(buf, len);

    if((size + reg_addr) > 256)
        size = 256 - reg_addr;

    printf("iic addr: 0x%02X; reg_addr:0x%02X; read len:%d\r\n", iic_addr, reg_addr, size);
    printf("addr 00 01 02 03 04 05 06 07 08 0A 0B 0C 0D 0E 0F");
    for(i = 0; i < size; i++)
    {
        if((i % 16) == 0)
        {
            printf("\r\n");
            printf("%04X ", i);
        }
        printf("%02X ", IIC_Read_Byte(IIC, iic_addr, reg_addr));
    }
    printf("\r\n");
}

void ADC_Process(uint8_t *buf, uint32_t len)
{
    printf("ADC Value: %d\r\n", Get_ADC(ADC_CHANNEL_1));
}










