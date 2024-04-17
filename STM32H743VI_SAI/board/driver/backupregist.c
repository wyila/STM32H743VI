#include "hal_sys.h"
#include "backupregist.h"



RTC_HandleTypeDef RTC_Handler;

void Backup_Register_Init(void)
{
//    RTC_Handler.Instance = RTC;
//    RTC_Handler.Init.HourFormat = RTC_HOURFORMAT_24;
//    RTC_Handler.Init.AsynchPrediv = 127;
//    RTC_Handler.Init.SynchPrediv = 255;
//    RTC_Handler.Init.OutPut = RTC_OUTPUT_DISABLE;
//    RTC_Handler.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
//    RTC_Handler.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
//    RTC_Handler.Init.OutPutRemap = RTC_OUTPUT_REMAP_NONE;
//    if (HAL_RTC_Init(&RTC_Handler) != HAL_OK)
//    {
//        Error_Handler();
//    }

//    HAL_RTCEx_BKUPWrite(&RTC_Handler, RTC_BKP_DR0, 0x08);

//    __HAL_RCC_BKPRAM_CLK_ENABLE();
//    HAL_PWREx_EnableBkUpReg();
}

