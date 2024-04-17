#include "task.h"
#include "cmd_process.h"


/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOD_CLK_ENABLE();
    __HAL_RCC_GPIOE_CLK_ENABLE();
    __HAL_RCC_GPIOH_CLK_ENABLE();
}

static IIC_GPIO_BASE iic;
static IIC_GPIO_BASE *IIC = &iic;
void BSP_Init(void)
{
//    MPU_Config();
    CM7_Core_Init();
    MX_GPIO_Init();
    
    LED_Init();
    Key_Init();
    TIM6_Init();
    TIM7_Init();
    delay_init();
    USART1_Init(115200);
    Backup_Register_Init();
//    ADC3_Init();
    IIC->frequency = 100000;
    IIC->SCL_PORT = GPIOE;
    IIC->SCL_PIN = GPIO_PIN_0;
    IIC->SDA_PORT = GPIOE;
    IIC->SDA_PIN = GPIO_PIN_1;
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



void USART_Task(void)
{
    if(USART1_RX_MQ_PI->state)
    {
        if(!memcmp(USART1_RX_MQ_PI->data, "iic_write", 9))
        {
            IIC_Write_Process(IIC, USART1_RX_MQ_PI->data, USART1_RX_MQ_PI->len);
        }
        else if(!memcmp(USART1_RX_MQ_PI->data, "iic_read", 8))
        {
            IIC_Read_Process(IIC, USART1_RX_MQ_PI->data, USART1_RX_MQ_PI->len);
        }
//        if(!memcmp(USART1_RX_MQ_PI->data, "get_adc", 7))
//        {
//            ADC_Process(USART1_RX_MQ_PI->data, USART1_RX_MQ_PI->len);
//        }
        USART1_RX_MQ_PI->state = 0;
        USART1_RX_MQ_PI = USART1_RX_MQ_PI->next;
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

