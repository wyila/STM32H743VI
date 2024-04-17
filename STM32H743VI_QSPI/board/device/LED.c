#include "hal_sys.h"
#include "GPIO_BSP.h"
#include "LED.h"


//SW: 0:关灯; 1:开灯;
void LED0_SW(uint8_t SW)
{
    if(SW)
        LED0_PORT->ODR |= LED0_PIN;
    else
        LED0_PORT->ODR &= ~LED0_PIN;
}

void LED0_Toggles(void)
{
    LED0_PORT->ODR ^= LED0_PIN;
}


void LED1_SW(uint8_t SW)
{
    if(SW)
        LED1_PORT->ODR |= LED1_PIN;
    else
        LED1_PORT->ODR &= ~LED1_PIN;
}

void LED1_Toggles(void)
{
    LED1_PORT->ODR ^= LED1_PIN;
}


void LED2_SW(uint8_t SW)
{
    if(SW)
        LED2_PORT->ODR |= LED2_PIN;
    else
        LED2_PORT->ODR &= ~LED2_PIN;
}

void LED2_Toggles(void)
{
    LED2_PORT->ODR ^= LED2_PIN;
}



void LED3_SW(uint8_t SW)
{
    if(SW)
        LED3_PORT->ODR |= LED3_PIN;
    else
        LED3_PORT->ODR &= ~LED3_PIN;
}

void LED3_Toggles(void)
{
    LED3_PORT->ODR ^= LED3_PIN;
}


void LED_ALL_SW(uint8_t state)
{
    LED0_SW(state);
//    LED1_SW(state);
//    LED2_SW(state);
//    LED3_SW(state);
}

void LED_ALL_Toggles()
{
    LED0_Toggles();
//    LED1_Toggles();
//    LED2_Toggles();
//    LED3_Toggles();
}


void LED_Init(void)
{
    LED_GPIO_Init();
    LED_ALL_SW(LED_OPEN);//开灯
}


