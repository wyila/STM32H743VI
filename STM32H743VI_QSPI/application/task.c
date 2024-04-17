#include "task.h"
#include "cmd_process.h"
#include "w25qxx.h"
#include "xmodem.h"

static IIC_GPIO_BASE iic;
static IIC_GPIO_BASE *IIC = &iic;
void BSP_Init(void)
{
//    MPU_Config();
    LED_Init();
    Key_Init();
    TIM6_Init();
    TIM7_Init();
    delay_init();
    USART1_Init(115200);
    Backup_Register_Init();
    
    W25QXX_Init();
    
//    ADC3_Init();
    IIC->frequency = 100000;
    IIC->SCL_PORT = GPIOE;
    IIC->SCL_PIN = GPIO_PIN_4;
    IIC->SDA_PORT = GPIOE;
    IIC->SDA_PIN = GPIO_PIN_5;
    IIC_Init(IIC);
}

void BSP_DeInit(void)
{
    USART_DeInit();
    TIM_DeInit();
    //Disable SysTick
    SysTick->CTRL  &= ~SysTick_CTRL_ENABLE_Msk;
    INTX_DISABLE();
}

void LED_Task(void)
{
    static uint32_t old_time;
    if(old_time != SysTime)
    {
        if((old_time % 1000) == 0)
        {
            LED_ALL_Toggles();
        }
        old_time = SysTime;
    }
}



void Xmodem_Copy_Data(uint8_t *buf, uint32_t len)
{
    static uint32_t index = 0;
    
    W25QXX_Write(buf, index, len);
    index += len;
}


void USART_Task(void)
{
    uint8_t *buf;
    uint32_t len;
    
    if(USART1_RX_MQ_PI->state)
    {
        buf = USART1_RX_MQ_PI->data;
        len = USART1_RX_MQ_PI->len;
        USART1_RX_MQ_PI->state = 0;
        USART1_RX_MQ_PI = USART1_RX_MQ_PI->next;
        
        if(!memcmp(buf, "iic_write", 9))
        {
            IIC_Write_Process(IIC, buf, len);
        }
        else if(!memcmp(buf, "iic_read", 8))
        {
            IIC_Read_Process(IIC, buf, len);
        }
        if(!memcmp(buf, "xmodem", 6))
        {
            Xmodem_Transfer();
        }
    }
}


void ADC_Task(void)
{
    static uint32_t old_time;
//    uint16_t ADC_VAL;
    
    if(old_time != SysTime)
    {
        old_time = SysTime;
        if((old_time % 1000) == 0)
        {
//            ADC_VAL = Get_ADC(ADC_CHANNEL_1);
//            printf("ADC: 0x%04X\r\n", ADC_VAL);
//            printf("Voltage: %.3f\r\n", ADC_VAL * 3.3 / 65535);
        }
    }
    
}

