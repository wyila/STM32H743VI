#include "hal_sys.h"
#include "usart.h"

UART_HandleTypeDef USART1_HAND = {0};

uint8_t USART1_RX_BUF[USART_BUF_SIZE];
uint32_t USART1_RX_LEN;//记录整体接收的长度
uint32_t USART1_RX_CNT;//记录单次接收的长度

UART_RX_QUEUE USART1_RX_FIFO[USART_FIFO_SIZE];
UART_RX_QUEUE *USART1_RX_MQ_PI = &USART1_RX_FIFO[0];
UART_RX_QUEUE *USART1_RX_INT_PI = &USART1_RX_FIFO[0];

UART_TX_QUEUE USART1_TX_FIFO[USART_FIFO_SIZE];
UART_TX_QUEUE *USART1_TX_SEND_PI = &USART1_TX_FIFO[0];
UART_TX_QUEUE *USART1_TX_INT_PI = &USART1_TX_FIFO[0];


void USART1_Init(uint32_t Baud_rate)
{
    USART1_HAND.Instance = USART1;
    USART1_HAND.Init.BaudRate = Baud_rate;
    USART1_HAND.Init.WordLength = UART_WORDLENGTH_8B;
    USART1_HAND.Init.StopBits = UART_STOPBITS_1;
    USART1_HAND.Init.Parity = UART_PARITY_NONE;
    USART1_HAND.Init.Mode = UART_MODE_TX_RX;
    USART1_HAND.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    USART1_HAND.Init.OverSampling = UART_OVERSAMPLING_16;
    USART1_HAND.Init.ClockPrescaler = UART_PRESCALER_DIV1;
    USART1_HAND.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
    USART1_HAND.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;

    MQ_Init(USART1_RX_FIFO, USART_FIFO_SIZE);
    UART_TX_Queue_Init(USART1_TX_FIFO, USART_FIFO_SIZE);

    HAL_UART_Init(&USART1_HAND);
    USART1->CR1 &= ~0x00000081;//关闭串口1、发送为空中断
    USART1->CR1 |= 0x0000007C;//使能空闲中断、接收中断、发送完成中断
    USART1->CR2 = 0;//处理一些不必要的干扰
    USART1->CR3 = 0;
    USART1->CR1 |= 0x00000001;//开启串口1
    HAL_NVIC_EnableIRQ(USART1_IRQn);//中断最后开，不然容易卡中断函数
}

uint8_t USART1_Send(uint8_t *buf, uint32_t len)
{
    uint32_t i, timeout;

    for(i = 0; i < len; i ++)
    {
        //如果当前节点状态为忙，证明整个队列已满
        timeout = SysTime;
        while(USART1_TX_SEND_PI->state)//等待队列清空
        {
            //如果队列满了，但是中断没开启
            if((USART1->CR1 & USART_IT_TXE) == 0)
                USART1->CR1 |= USART_IT_TXE;//开启发送为空中断
            if((SysTime - timeout) > 100)//超时
                return 0;
        }
        USART1_TX_SEND_PI->data = buf[i];
        USART1_TX_SEND_PI->state = 1;
        USART1_TX_SEND_PI = USART1_TX_SEND_PI->next;
    }
    if((USART1->CR1 & USART_IT_TXE) == 0)
        USART1->CR1 |= USART_IT_TXE;

    return i;
}

void USART1_IRQHandler(void)
{
    uint32_t tmpreg;

    if(USART1->ISR & USART_IT_RXNE)//检测到接收中断
    {
        USART1_RX_BUF[USART1_RX_LEN++] = USART1->RDR;
        USART1_RX_CNT++;
        if(USART1_RX_LEN >= USART_BUF_SIZE)
        {
            if(USART1_RX_CNT > USART_BUF_SIZE)
            {
                USART1_RX_CNT = 0;
                USART1_RX_LEN = 0;
            }
            memcpy(USART1_RX_BUF, &USART1_RX_BUF[USART1_RX_LEN - USART1_RX_CNT], USART1_RX_CNT);
            USART1_RX_LEN = USART1_RX_CNT;
        }
    }
    //发送数据寄存器为空
    if(USART1->ISR & USART_IT_TXE)
    {
        if(USART1_TX_INT_PI->state)
        {
            USART1->TDR = USART1_TX_INT_PI->data;
            USART1_TX_INT_PI->state = 0;
            USART1_TX_INT_PI = USART1_TX_INT_PI->next;
        }
        else
        {
            USART1->CR1 &= ~USART_IT_TXE;//关闭发送为空中断
        }
    }

    //检测到发送完成中断
    if(USART1->ISR & USART_IT_TC)
    {
        USART1->CR1 &= ~USART_IT_TXE;//关闭发送为空中断
        USART1->ICR |= USART_IT_TC;//清除发送标志
    }
    //总线空闲
    if(USART1->ISR & USART_IT_IDLE)
    {
        USART1->ICR |= USART_IT_IDLE;

        if((USART1_RX_CNT != 0) && (USART1_RX_CNT <= USART1_RX_LEN))
        {
            USART1_RX_INT_PI->data = &USART1_RX_BUF[USART1_RX_LEN - USART1_RX_CNT];
            USART1_RX_INT_PI->len = USART1_RX_CNT;
            USART1_RX_INT_PI->state = 1;
            USART1_RX_INT_PI = USART1_RX_INT_PI->next;
            USART1_RX_CNT = 0;
        }
    }

    //除去接收中断、发送缓冲为空中断、发送完成中断、空闲中断
    //其他统一清零
    tmpreg = USART1->ISR & 0x123B0F;
    if(tmpreg)
    {
        USART1->ICR |= tmpreg;
    }

//    //奇偶校验错误
//    if(USART1->ISR & USART_IT_PE)
//    {
//        USART1->ICR &= ~USART_IT_PE;
//    }
//    //帧错误
//    if(USART1->ISR & USART_IT_FE)
//    {
//        USART1->ICR &= ~USART_IT_FE;
//    }
//    //检测到噪声
//    if(USART1->ISR & USART_IT_NF)
//    {
//        USART1->ICR &= ~USART_IT_NF;
//    }
//    //溢出中断
//    if(USART1->ISR & USART_IT_ORE)
//    {
//        USART1->ICR &= ~USART_IT_ORE;
//    }
}

/**
* @brief UART MSP Initialization
* This function configures the hardware resources used in this example
* @param huart: UART handle pointer
* @retval None
*/
void HAL_UART_MspInit(UART_HandleTypeDef* huart)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};
    if(huart->Instance == USART1)
    {
        /** Initializes the peripherals clock */
        PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_USART1;
        PeriphClkInitStruct.Usart16ClockSelection = RCC_USART16CLKSOURCE_D2PCLK2;

        if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
        {
            Error_Handler();
        }
        /* Peripheral clock enable */
        __HAL_RCC_USART1_CLK_ENABLE();

        __HAL_RCC_GPIOA_CLK_ENABLE();
        __HAL_RCC_GPIOB_CLK_ENABLE();
        /**USART1 GPIO Configuration
        PA9     ------> USART1_TX
        PA10    ------> USART1_RX */
        GPIO_InitStruct.Pin = GPIO_PIN_9|GPIO_PIN_10;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
        GPIO_InitStruct.Alternate = GPIO_AF7_USART1;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
        
        /* USART1 interrupt Init */
        HAL_NVIC_SetPriority(USART1_IRQn, 0, 0);
        HAL_NVIC_DisableIRQ(USART1_IRQn);
    }
}

/**
* @brief UART MSP De-Initialization
* This function freeze the hardware resources used in this example
* @param huart: UART handle pointer
* @retval None
*/
void HAL_UART_MspDeInit(UART_HandleTypeDef* huart)
{
    if(huart->Instance == USART1)
    {
        __HAL_RCC_USART1_CLK_DISABLE();
        //PD8: TX; PD9: RX;
        HAL_GPIO_DeInit(GPIOA, GPIO_PIN_10 | GPIO_PIN_9);
        HAL_NVIC_DisableIRQ(USART1_IRQn);
    }
}




int fputc(int ch, FILE *f)
{
    uint32_t timeout;

    //此函数调用频繁，不需要考虑数据完整性，不然容易卡死
    timeout = SysTime;
    while(USART1_TX_SEND_PI->state)//等待队列清空
    {
        //如果队列满了，但是中断没开启
        if((USART1->CR1 & USART_IT_TXE) == 0)
            USART1->CR1 |= USART_IT_TXE;//开启发送为空中断
        if((SysTime - timeout) > 100)//超时
            return 0;
    }
    USART1_TX_SEND_PI->data = ch & 0xFF;
    USART1_TX_SEND_PI->state = 1;
    USART1_TX_SEND_PI = USART1_TX_SEND_PI->next;

    if((USART1->CR1 & USART_IT_TXE) == 0)
        USART1->CR1 |= USART_IT_TXE;

    //不使用队列的实现方式
//    USART1->TDR = (ch & 0xFF);
//    while((USART1->ISR & 0x80) == 0);//等待发送区为空

	return ch;
}

void USART_DeInit(void)
{
    HAL_UART_DeInit(&USART1_HAND);
}


