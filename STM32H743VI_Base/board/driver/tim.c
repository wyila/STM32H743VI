#include "hal_sys.h"
#include "tim.h"

TIM_HandleTypeDef TIM6_Handler, TIM7_Handler;

void TIM6_Init(void)
{
    TIM_MasterConfigTypeDef sMasterConfig = {0};
    TIM6_Handler.Instance = TIM6;
    TIM6_Handler.Init.Prescaler = 1999;
    TIM6_Handler.Init.CounterMode = TIM_COUNTERMODE_UP;
    TIM6_Handler.Init.Period = 9999;
    TIM6_Handler.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
    if (HAL_TIM_Base_Init(&TIM6_Handler) != HAL_OK)
    {
        Error_Handler();
    }
    sMasterConfig.MasterOutputTrigger = TIM_TRGO_UPDATE;
    sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
    if (HAL_TIMEx_MasterConfigSynchronization(&TIM6_Handler, &sMasterConfig) != HAL_OK)
    {
        Error_Handler();
    }
    TIM6->DIER = 0x01;
    TIM6->CR1 |= 0x01;
}


void TIM7_Init(void)
{
    TIM_MasterConfigTypeDef sMasterConfig = {0};

    TIM7_Handler.Instance = TIM7;
    TIM7_Handler.Init.Prescaler = 1999;
    TIM7_Handler.Init.CounterMode = TIM_COUNTERMODE_UP;
    TIM7_Handler.Init.Period = 99;
    TIM7_Handler.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
    if (HAL_TIM_Base_Init(&TIM7_Handler) != HAL_OK)
    {
        Error_Handler();
    }
    sMasterConfig.MasterOutputTrigger = TIM_TRGO_UPDATE;
    sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
    if (HAL_TIMEx_MasterConfigSynchronization(&TIM7_Handler, &sMasterConfig) != HAL_OK)
    {
        Error_Handler();
    }
    TIM7->DIER = 0x01;
    TIM7->CR1 |= 0x01;
}

void TIM_DeInit(void)
{
    HAL_TIM_Base_DeInit(&TIM6_Handler);
    HAL_TIM_Base_DeInit(&TIM7_Handler);
}
