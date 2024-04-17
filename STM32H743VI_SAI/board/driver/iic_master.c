#include "iic_master.h"

//软件模拟IIC

//设备引脚初始化
void IIC_Init(struct IIC_GPIO *IIC)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};

    GPIO_InitStructure.Pin = IIC->SCL_PIN;;
    GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_OD;
    GPIO_InitStructure.Pull = GPIO_PULLUP;
    GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    HAL_GPIO_Init(IIC->SCL_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.Pin = IIC->SDA_PIN;
    GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_OD;
    GPIO_InitStructure.Pull = GPIO_PULLUP;
    GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    HAL_GPIO_Init(IIC->SDA_PORT, &GPIO_InitStructure);

    IIC->SDA_PORT->ODR |= IIC->SDA_PIN;
    IIC->SCL_PORT->ODR |= IIC->SCL_PIN;
    if(IIC->frequency > 500000)
        IIC->frequency = 500000;

}


//设置SDA模式
void IIC_SDA_SET(struct IIC_GPIO *IIC, unsigned char state)
{
    uint16_t tmp_gpio = IIC->SDA_PIN;
    uint8_t gpio_num = 0;
    while(tmp_gpio >>= 1)
    {
        gpio_num ++;
    }
    gpio_num *= 2;

    if(IIC->SDA_PORT->IDR & IIC->SDA_PIN)
        IIC->SDA_PORT->ODR |= IIC->SDA_PIN;
    else
        IIC->SDA_PORT->ODR &= ~IIC->SDA_PIN;
    IIC->SDA_PORT->MODER &= ~(0x03 << gpio_num);
    if(state)//如果设置成输出
        IIC->SDA_PORT->MODER |= (0x01 << gpio_num);
}

void IIC_SCL(struct IIC_GPIO *IIC, unsigned char state)
{
    if(state)
        HAL_GPIO_WritePin(IIC->SCL_PORT, IIC->SCL_PIN, GPIO_PIN_SET);
    else
        HAL_GPIO_WritePin(IIC->SCL_PORT, IIC->SCL_PIN, GPIO_PIN_RESET);
}

void IIC_SDA(struct IIC_GPIO *IIC, unsigned char state)
{
    if(state)
        HAL_GPIO_WritePin(IIC->SDA_PORT, IIC->SDA_PIN, GPIO_PIN_SET);
    else
        HAL_GPIO_WritePin(IIC->SDA_PORT, IIC->SDA_PIN, GPIO_PIN_RESET);
}

int IIC_SDA_STATE(struct IIC_GPIO *IIC)
{
    return HAL_GPIO_ReadPin(IIC->SDA_PORT, IIC->SDA_PIN);
}

void IIC_START(struct IIC_GPIO *IIC)
{
    IIC_SDA_SET(IIC, 1);
    IIC_SDA(IIC, 1);
    IIC_SCL(IIC, 1);
    delay_us((1000000 / IIC->frequency));
    IIC_SDA(IIC, 0);
    delay_us((1000000 / IIC->frequency) / 2);
    IIC_SCL(IIC, 0);
    delay_us((1000000 / IIC->frequency) / 2);
}


void IIC_END(struct IIC_GPIO *IIC)
{
    IIC_SCL(IIC, 0);
    IIC_SDA_SET(IIC, 1);
    IIC_SDA(IIC, 0);
    delay_us((1000000 / IIC->frequency));
    IIC_SCL(IIC, 1);
    delay_us((1000000 / IIC->frequency) / 2);
    IIC_SDA(IIC, 1);
    delay_us((1000000 / IIC->frequency));
}

void IIC_Read_ACK(struct IIC_GPIO *IIC)
{
    IIC_SCL(IIC, 0);
    IIC_SDA_SET(IIC, 1);
    IIC_SDA(IIC, 0);
    delay_us((1000000 / IIC->frequency));
    IIC_SCL(IIC, 1);
    delay_us((1000000 / IIC->frequency));
    IIC_SCL(IIC, 0);
}



void IIC_Read_NAK(struct IIC_GPIO *IIC)
{
    IIC_SCL(IIC, 0);
    IIC_SDA_SET(IIC, 1);
    IIC_SDA(IIC, 1);
    delay_us((1000000 / IIC->frequency));
    IIC_SCL(IIC, 1);
    delay_us((1000000 / IIC->frequency));
    IIC_SCL(IIC, 0);
}

int IIC_Check_Ack(struct IIC_GPIO *IIC)
{
    int ret;

    IIC_SCL(IIC, 0);
    IIC_SDA_SET(IIC, 0);//设置为输入
    delay_us((1000000 / IIC->frequency));

    IIC_SCL(IIC, 1);
    ret = IIC_SDA_STATE(IIC);
    delay_us((1000000 / IIC->frequency));

    IIC_SCL(IIC, 0);

    return ret;
}

int IIC_Write_Base(struct IIC_GPIO *IIC, unsigned char data)
{
    int i;

    IIC_SCL(IIC, 0);
    IIC_SDA_SET(IIC, 1);

    for(i = 0; i < 8; i++)
    {
        IIC_SCL(IIC, 0);

        if(data & 0x80)
            IIC_SDA(IIC, 1);
        else
            IIC_SDA(IIC, 0);
        data <<= 1;

        delay_us((1000000 / IIC->frequency));

        IIC_SCL(IIC, 1);
        delay_us((1000000 / IIC->frequency));
    }
    IIC_SCL(IIC, 0);

    return 1;
}

unsigned char IIC_Read_Base(struct IIC_GPIO *IIC)
{
    unsigned data = 0;
    int i;

    IIC_SCL(IIC, 0);
    IIC_SDA_SET(IIC, 0);//设置为输入

    for(i = 0; i < 8; i++)
    {
        IIC_SCL(IIC, 0);
        delay_us((1000000 / IIC->frequency));

        data <<= 1;
        if(IIC_SDA_STATE(IIC))
            data |= 1;
        IIC_SCL(IIC, 1);
        delay_us((1000000 / IIC->frequency));
    }
    IIC_SCL(IIC, 0);

    return data;
}


int IIC_Write_Byte(struct IIC_GPIO *IIC, unsigned char IIC_ADDR, unsigned char IIC_Sub_Addr, unsigned char data)
{
    IIC_START(IIC);//起始信号
    IIC_Write_Base(IIC, IIC_ADDR << 1);//IIC硬件地址
    if(IIC_Check_Ack(IIC) == IIC_NAK)//检查是否有回应
    {
        IIC_END(IIC);//没有就结束
        return 0;
    }
    IIC_Write_Base(IIC, IIC_Sub_Addr);//IIC寄存器地址
    if(IIC_Check_Ack(IIC) == IIC_NAK)//同上
    {
        IIC_END(IIC);
        return 0;
    }

    IIC_Write_Base(IIC, data);
    if(IIC_Check_Ack(IIC) == IIC_NAK)
    {
        IIC_END(IIC);
        return 1;
    }
    IIC_END(IIC);
    return 1;//返回写入了多少个字节
}

unsigned char IIC_Read_Byte(struct IIC_GPIO *IIC, unsigned char IIC_ADDR, unsigned char IIC_Sub_Addr)
{
    unsigned char data;
    IIC_START(IIC);//起始信号
    IIC_Write_Base(IIC, IIC_ADDR << 1);//IIC硬件地址
    if(IIC_Check_Ack(IIC) == IIC_NAK)
    {
        IIC_END(IIC);
        return 0;
    }
    IIC_Write_Base(IIC, IIC_Sub_Addr);//IIC寄存器地址
    if(IIC_Check_Ack(IIC) == IIC_NAK)
    {
        IIC_END(IIC);
        return 0;
    }

    IIC_END(IIC);//结束

    IIC_START(IIC);//开始读数据
    IIC_Write_Base(IIC, (IIC_ADDR << 1) | 1);//IIC硬件地址
    if(IIC_Check_Ack(IIC) == IIC_NAK)
    {
        IIC_END(IIC);
        return 0;
    }
    data = IIC_Read_Base(IIC);//最后一个数据回复NAK
    IIC_Read_NAK(IIC);
    IIC_END(IIC);//真正结束
    return data;
}


int IIC_Write(struct IIC_GPIO *IIC, unsigned char IIC_ADDR, unsigned char IIC_Sub_Addr, const unsigned char *buf, unsigned int len)
{
    int i;

    IIC_START(IIC);//起始信号
    IIC_Write_Base(IIC, IIC_ADDR << 1);//IIC硬件地址
    if(IIC_Check_Ack(IIC) == IIC_NAK)//检查是否有回应
    {
        IIC_END(IIC);//没有就结束
        return 0;
    }
    IIC_Write_Base(IIC, IIC_Sub_Addr);//IIC寄存器地址
    if(IIC_Check_Ack(IIC) == IIC_NAK)//同上
    {
        IIC_END(IIC);
        return 0;
    }

    for(i = 0; i < len; i++)//循环写入数据
    {
        IIC_Write_Base(IIC, buf[i]);
        if(IIC_Check_Ack(IIC) == IIC_NAK)
        {
            IIC_END(IIC);
            return i;
        }
    }
    IIC_END(IIC);
    return i;//返回写入了多少个字节
}



int IIC_Read(struct IIC_GPIO *IIC, unsigned char IIC_ADDR, unsigned char IIC_Sub_Addr, unsigned char *buf, unsigned int len)
{
    int i;

    IIC_START(IIC);//起始信号
    IIC_Write_Base(IIC, IIC_ADDR << 1);//IIC硬件地址
    if(IIC_Check_Ack(IIC) == IIC_NAK)
    {
        IIC_END(IIC);
        return 0;
    }
    IIC_Write_Base(IIC, IIC_Sub_Addr);//IIC寄存器地址
    if(IIC_Check_Ack(IIC) == IIC_NAK)
    {
        IIC_END(IIC);
        return 0;
    }
    IIC_END(IIC);//结束

    IIC_START(IIC);//开始读数据
    IIC_Write_Base(IIC, (IIC_ADDR << 1) | 1);//IIC硬件地址
    if(IIC_Check_Ack(IIC) == IIC_NAK)
    {
        IIC_END(IIC);
        return 0;
    }
    for(i = 0; i < len - 1; i++)//循环读数据
    {
        buf[i] = IIC_Read_Base(IIC);
        IIC_Read_ACK(IIC);
    }
    buf[i] = IIC_Read_Base(IIC);
    IIC_Read_NAK(IIC);
    IIC_END(IIC);//结束
    return i;
}





