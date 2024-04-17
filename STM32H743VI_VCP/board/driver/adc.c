#include "hal_sys.h"
#include "adc.h"

ADC_HandleTypeDef ADC3_Handler;

/**
  * @brief ADC3 Initialization Function
  * @param None
  * @retval None
  */
void ADC3_Init(void)
{
    ADC_ChannelConfTypeDef sConfig = {0};

    ADC3_Handler.Instance = ADC3;
    ADC3_Handler.Init.Resolution = ADC_RESOLUTION_16B;
    ADC3_Handler.Init.ScanConvMode = ADC_SCAN_DISABLE;
    ADC3_Handler.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
    ADC3_Handler.Init.LowPowerAutoWait = DISABLE;
    ADC3_Handler.Init.ContinuousConvMode = DISABLE;
    ADC3_Handler.Init.NbrOfConversion = 1;
    ADC3_Handler.Init.DiscontinuousConvMode = DISABLE;
    ADC3_Handler.Init.ExternalTrigConv = ADC_SOFTWARE_START;
    ADC3_Handler.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
    ADC3_Handler.Init.ConversionDataManagement = ADC_CONVERSIONDATA_DR;
    ADC3_Handler.Init.Overrun = ADC_OVR_DATA_PRESERVED;
    ADC3_Handler.Init.LeftBitShift = ADC_LEFTBITSHIFT_NONE;
    ADC3_Handler.Init.OversamplingMode = DISABLE;
    if(HAL_ADC_Init(&ADC3_Handler) != HAL_OK)
    {
        Error_Handler();
    }

    sConfig.Channel = ADC_CHANNEL_1;
    sConfig.Rank = ADC_REGULAR_RANK_1;
    sConfig.SamplingTime = ADC_SAMPLETIME_1CYCLE_5;
    sConfig.SingleDiff = ADC_SINGLE_ENDED;
    sConfig.OffsetNumber = ADC_OFFSET_NONE;
    sConfig.Offset = 0;
    sConfig.OffsetSignedSaturation = DISABLE;
    if(HAL_ADC_ConfigChannel(&ADC3_Handler, &sConfig) != HAL_OK)
    {
        Error_Handler();
    }
    //ADC校验
    HAL_ADCEx_Calibration_Start(&ADC3_Handler, ADC_CALIB_OFFSET, ADC_SINGLE_ENDED);
}

uint16_t Get_ADC(uint32_t ch)
{
//    ADC_ChannelConfTypeDef ADC1_ChanConf;

//    ADC1_ChanConf.Channel = ch;//通道
//    ADC1_ChanConf.Rank = ADC_REGULAR_RANK_1;//1个序列
//    ADC1_ChanConf.SamplingTime = ADC_SAMPLETIME_387CYCLES_5;//采样时间
//    ADC1_ChanConf.SingleDiff = ADC_SINGLE_ENDED;//单边采集
//    ADC1_ChanConf.OffsetNumber = ADC_OFFSET_NONE;
//    ADC1_ChanConf.Offset = 0;
//    HAL_ADC_ConfigChannel(&ADC3_Handler,&ADC1_ChanConf);//通道配置

    HAL_ADC_Start(&ADC3_Handler);//开启ADC

    //HAL_ADC_PollForConversion(&ADC1_Handler, 10);//轮询转换
	return (uint16_t)HAL_ADC_GetValue(&ADC3_Handler);//返回最近一次ADC1规则组的转换结果
}


