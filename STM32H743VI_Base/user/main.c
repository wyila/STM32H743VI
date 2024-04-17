#include "main.h"
#include "task.h"

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
    BSP_Init();

    while (1)
    {
        LED_Task();
        USART_Task();
        ADC_Task();
    }
}






