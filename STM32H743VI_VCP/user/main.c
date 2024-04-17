#include "main.h"
#include "task.h"


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


/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
    CM7_Core_Init();

    MX_GPIO_Init();
    BSP_Init();

    while (1)
    {
        LED_Task();
        USART_Task();
        ADC_Task();
    }
}






