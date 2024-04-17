#include "task.h"
#include "cmd_process.h"
#include "w25qxx.h"
#include "xmodem.h"


#include "sai.h"


#define AUDIO_SIZE      3145728

//uint8_t audio[100 * 1024] __attribute__((at(0x24000000)));
typedef struct audio_base{
    uint8_t buf[1024];
    uint8_t state;//0:none; 1:就绪; 2:正在使用;
    struct audio_base *next;
}Audio_TypeDef;

Audio_TypeDef audio_pool[2] = {0};

Audio_TypeDef *Audio;
Audio_TypeDef *AudioScan;

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
//    Backup_Register_Init();
    
    W25QXX_Init();
    
    SAI1A_Init();
    SAIA_SampleRate_Set(32000);
    
    
//    ADC3_Init();
    IIC->frequency = 100000;
    IIC->SCL_PORT = GPIOE;
    IIC->SCL_PIN = GPIO_PIN_0;
    IIC->SDA_PORT = GPIOE;
    IIC->SDA_PIN = GPIO_PIN_1;
    IIC_Init(IIC);
    audio_pool[0].next = &audio_pool[1];
    audio_pool[1].next = &audio_pool[0];
    Audio = &audio_pool[0];
    AudioScan = Audio;
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

extern SAI_HandleTypeDef SAI1A_Handler;


void SAI_Task(void)
{
    static uint16_t data = 0xFFFF;
    static uint32_t tmp, reg;
    static uint32_t index = 0;
    static uint32_t buf_index;
    
    if(SAI1_Block_A->SR & 0x08)//检查数据是否可以入队
    {
        SAI1_Block_A->DR = data;
    }
    reg = SAI1_Block_A->SR & 0x77;
    if(reg)
        SAI1_Block_A->CLRFR = reg;
    #if 0
    if(index < AUDIO_SIZE)
    {
        if(AudioScan->state == 0)
        {
            //W25QXX_Read(AudioScan->buf, index, 1024);
            memset(AudioScan->buf, 0xFF, 1024);
            index += 1024;
            AudioScan->state = 1;
            AudioScan = AudioScan->next;
        }
        reg = SAI1_Block_A->SR & 0x77;
        if(reg)
            SAI1_Block_A->CLRFR = reg;
        if(SAI1_Block_A->SR & 0x08)//检查数据是否可以入队
        {
            if(Audio->state == 1)
            {
                Audio->state = 2;
                buf_index = 0;
                if((SAI1_Block_A->SR & 0x10000) == 0)
                    __HAL_SAI_ENABLE(&SAI1A_Handler);//开启SAI
            }
            if(Audio->state == 2)
            {
                SAI1_Block_A->DR = Audio->buf[buf_index ++];
                if(buf_index >= 1024)
                {
                    buf_index = 0;
                    Audio->state = 0;
                    Audio = Audio->next;
                }
            }
            if(Audio->state == 0)
            {
                if(SAI1_Block_A->SR & 0x10000)
                    __HAL_SAI_DISABLE(&SAI1A_Handler);//关闭SAI
            }
        }
    }
    
    #endif
}






