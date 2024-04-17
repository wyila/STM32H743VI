#include "hal_sys.h"
#include "GPIO_BSP.h"
#include "Key.h"

Key_TypeDef key1;

//按键扫描
void Key_Scan(void)
{
    uint8_t tmp;

    uint32_t current_time = SysTime;

    tmp = HAL_GPIO_ReadPin(KEY_PORT, KEY_PIN);

    if(key1.val != tmp)
    {
        key1.val = tmp;
        key1.trigger_time = SysTime;
        key1.keep_time = 0;
    }
    else
    {
        key1.keep_time = SysTime - key1.trigger_time;
        if(key1.keep_time > 20)
        {
            if(key1.val == 0)
                key1.even = 1;
            else
                key1.even = 0;
        }
    }

}


void Key_Init(void)
{
    Key_GPIO_Init();
}



