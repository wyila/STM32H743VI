#ifndef __IIC_MASTER_H
#define __IIC_MASTER_H

#include "hal_sys.h"

#define IIC_ACK     0
#define IIC_NAK     1


typedef struct IIC_GPIO
{
    GPIO_TypeDef *SCL_PORT;
    uint16_t SCL_PIN;
    GPIO_TypeDef *SDA_PORT;
    uint16_t SDA_PIN;
    uint32_t frequency;
}IIC_GPIO_BASE;


//硬件层
void IIC_Init(struct IIC_GPIO *IIC);
void IIC_SDA_SET(struct IIC_GPIO *IIC, unsigned char state);
void IIC_SCL(struct IIC_GPIO *IIC, unsigned char state);
void IIC_SDA(struct IIC_GPIO *IIC, unsigned char state);
int IIC_SDA_STATE(struct IIC_GPIO *IIC);

//逻辑层
void IIC_START(struct IIC_GPIO *IIC);
void IIC_END(struct IIC_GPIO *IIC);
void IIC_Read_ACK(struct IIC_GPIO *IIC);
void IIC_Read_NAK(struct IIC_GPIO *IIC);
int IIC_Check_Ack(struct IIC_GPIO *IIC);
int IIC_Write_Base(struct IIC_GPIO *IIC, unsigned char data);
unsigned char IIC_Read_Base(struct IIC_GPIO *IIC);


//用户层
int IIC_Write_Byte(struct IIC_GPIO *IIC, unsigned char IIC_ADDR, unsigned char IIC_Sub_Addr, unsigned char data);
unsigned char IIC_Read_Byte(struct IIC_GPIO *IIC, unsigned char IIC_ADDR, unsigned char IIC_Sub_Addr);
int IIC_Write(struct IIC_GPIO *, unsigned char IIC_ADDR, unsigned char IIC_Sub_Addr, const unsigned char *buf, unsigned int len);
int IIC_Read(struct IIC_GPIO *, unsigned char IIC_ADDR, unsigned char IIC_Sub_Addr, unsigned char *buf, unsigned int len);



#endif






