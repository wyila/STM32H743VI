#include "hal_sys.h"
#include "stm32h7xx_hal.h"

#ifdef RTOS
#include "FreeRTOS.h"
#include "task.h"
#endif

static unsigned int fac_us;
static unsigned int fac_ms;

volatile unsigned int SysTime = 0;
static unsigned char delay_iden = 0;

void delay_init(void)
{
    SystemCoreClock = HAL_RCC_GetSysClockFreq();
    HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);
    HAL_SYSTICK_Config(SystemCoreClock / 1000);//1ms 中断一次

    fac_ms = SysTick->LOAD;
    if((fac_ms > 1000))
        fac_us = ((fac_ms + 1) / 1000) - 1;
    else
        fac_us = 480;
    delay_iden = 1;
    SysTime = 0;
}

void delay_us(unsigned int us)
{
    unsigned int first_val = SysTick->VAL;//记录刚进来的时候定时器的值
    unsigned int ms = SysTime;

    if(delay_iden == 0)
    {
        delay_init();
        delay_iden = 1;
    }
    if(us == 0)
        return;

    if(us >= 1000)//超过1ms
    {
        delay_ms(us / 1000);//机智
        ms = SysTime;//更新时间
        us %= 1000;//求余
    }

    us *= fac_us;//取滴答定时器延时x us的实际值
    if(us > first_val)
    {
        ms ++;
        while(ms > SysTime);//等待定时器重置
        us -= first_val;
        first_val = fac_ms;//这里fan_ms用SysTick->LOAD更标准
    }
    //到了这里SysTick->Val一定不会比us小
    us = first_val - us;//定时器计数到这，就表示延时完成
    if(us < 5)
        us = 5;//防止卡bug
    //这里us不能等于0，会出事，最好在5以上，会有5us的误差
    //实测us = 1也不是不行，频率高了才不会卡BUG
    while(SysTick->VAL > us);//等待最后延时
}

#ifdef RTOS
void delay_ms(unsigned int ms)
{
    vTaskDelay(ms);
}
#else
void delay_ms(unsigned int ms)
{
    //记录刚进来的时候定时器的值，实测有1us误差，在这里消除
    unsigned int first_val = SysTick->VAL;

    if(delay_iden == 0)
    {
        delay_init();
        delay_iden = 1;
    }
    if(ms == 0)
        return;
    ms += SysTime;
    //不正常情况（SysTime这个变量总有溢出的一天，虽说是小概率。或者说你并没有开启滴答定时器）
    if(ms < SysTime)
        while(ms < SysTime);//等待SysTime归零
    //下面是正常情况
    //等待延时
    while(ms > SysTime);
    //延时差不多达标，但又没完全达标
    //既然程序能走到这里，SysTick->VAL的值一定等于fac_ms，或者稍微比它小一点
    if(first_val < 5)
        first_val = 5;//防止卡bug
    while(SysTick->VAL > first_val);
}
#endif

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    HAL_PWREx_ConfigSupply(PWR_LDO_SUPPLY);

    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

    while(!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}

    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI48|RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_ON;
    RCC_OscInitStruct.LSEState = RCC_LSE_ON;
    RCC_OscInitStruct.HSI48State = RCC_HSI48_ON;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLM = 5;
    RCC_OscInitStruct.PLL.PLLN = 160;
    RCC_OscInitStruct.PLL.PLLP = 2;
    RCC_OscInitStruct.PLL.PLLQ = 16;
    RCC_OscInitStruct.PLL.PLLR = 4;
    RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1VCIRANGE_2;
    RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1VCOWIDE;
    RCC_OscInitStruct.PLL.PLLFRACN = 0;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        Error_Handler();
    }

    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
        |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2
        |RCC_CLOCKTYPE_D3PCLK1|RCC_CLOCKTYPE_D1PCLK1;
        RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
        RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
        RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV2;
        RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV2;
        RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV2;
        RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV2;
        RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV2;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
    {
        Error_Handler();
    }
    __HAL_RCC_D2SRAM1_CLK_ENABLE();
    __HAL_RCC_D2SRAM2_CLK_ENABLE();
    __HAL_RCC_D2SRAM3_CLK_ENABLE();
}



void MPU_Config(void)
{
    MPU_Region_InitTypeDef MPU_InitStruct = {0};

    /* Disables the MPU */
    HAL_MPU_Disable();

    /** Initializes and configures the Region and the memory to be protected */
    MPU_InitStruct.Enable = MPU_REGION_ENABLE;
    MPU_InitStruct.Number = MPU_REGION_NUMBER0;
    MPU_InitStruct.BaseAddress = 0x20000000;
    MPU_InitStruct.Size = MPU_REGION_SIZE_8KB;
    MPU_InitStruct.SubRegionDisable = 0x00;
    MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL1;
    MPU_InitStruct.AccessPermission = MPU_REGION_FULL_ACCESS;
    MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_ENABLE;
    MPU_InitStruct.IsShareable = MPU_ACCESS_NOT_SHAREABLE;
    MPU_InitStruct.IsCacheable = MPU_ACCESS_CACHEABLE;
    MPU_InitStruct.IsBufferable = MPU_ACCESS_BUFFERABLE;

    HAL_MPU_ConfigRegion(&MPU_InitStruct);
    /* Enables the MPU */
    HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);
}

void CM7_Core_Init(void)
{
//    CPU_CACHE_Enable();

    HAL_Init();
    //HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);

    /* Configure the system clock */
    SystemClock_Config();
}


/**
* @brief  CPU L1-Cache enable.
* @param  None
* @retval None
*/
void CPU_CACHE_Enable(void)
{
    SCB_EnableICache();/* Enable I-Cache */
    SCB_EnableDCache();/* Enable D-Cache */
    SCB->CACR |= 1 << 2;//强制D-Cache透写
}



int string_to_number(uint8_t *buf, uint8_t len)
{
    uint8_t i;
    int ret = 0;

    //转16进制
    if(!memcmp(buf, "0x", 2))
    {
        for(i = 2; i < len; i++)
        {
            ret <<= 4;
            if((buf[i] >= '0') && (buf[i] <= '9'))
                ret |= buf[i] & ~0xF0;
            else if((buf[i] >= 'A') && (buf[i] <= 'F'))
                ret |= (buf[i] - 'A' + 0x0A);
            else if((buf[i] >= 'a') && (buf[i] <= 'f'))
                ret |= (buf[i] - 'a' + 0x0A);
            else
                return ret >> 4;
        }
    }
    else if(buf[0] == '-')//负数十进制
    {
        for(i = 1; i < len; i++)
        {
            ret *= 10;
            if((buf[i] >= '0') && (buf[i] <= '9'))
                ret += (buf[i] & ~0xF0);
            else
                return -1 * (ret / 10);
        }
        ret = ret * (-1);
    }
    else//正数十进制
    {
        for(i = 0; i < len; i++)
        {
            ret *= 10;
            if((buf[i] >= '0') && (buf[i] <= '9'))
                ret += (buf[i] & ~0xF0);
            else
                return ret / 10;
        }
    }

    return ret;
}



/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
    /* USER CODE BEGIN Error_Handler_Debug */
    /* User can add his own implementation to report the HAL error return state */
    __disable_irq();
    while (1)
    {
    }
    /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */


#if defined(__clang__) //使用V6编译器(clang)
//THUMB指令不支持汇编内联
//采用如下方法实现执行汇编指令WFI
void __attribute__((noinline)) WFI_SET(void)
{
    __asm__("wfi");
}

//关闭所有中断(但是不包括fault和NMI中断)
void __attribute__((noinline)) INTX_DISABLE(void)
{
    __asm__("cpsid i \t\n"
            "bx lr");
}

//开启所有中断
void __attribute__((noinline)) INTX_ENABLE(void)
{
    __asm__("cpsie i \t\n"
            "bx lr");
}

//设置栈顶地址
//addr:栈顶地址
void __attribute__((noinline)) MSR_MSP(u32 addr)
{
    __asm__("msr msp, r0 \t\n"
            "bx r14");
}
#elif defined (__CC_ARM)    //使用V5编译器(ARMCC)

//THUMB指令不支持汇编内联
//采用如下方法实现执行汇编指令WFI
__asm void WFI_SET(void)
{
	WFI;
}
//关闭所有中断(但是不包括fault和NMI中断)
__asm void INTX_DISABLE(void)
{
	CPSID   I
	BX      LR
}
//开启所有中断
__asm void INTX_ENABLE(void)
{
	CPSIE   I
	BX      LR
}
//设置栈顶地址
//addr:栈顶地址
__asm void MSR_MSP(u32 addr)
{
	MSR MSP, r0 			//set Main Stack value
	BX r14
}
#endif


