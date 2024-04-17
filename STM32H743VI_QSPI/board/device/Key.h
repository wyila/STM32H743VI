#ifndef __KEY_H
#define __KEY_H

typedef struct Key_Base
{
    uint8_t even : 1;//发生按键事件
    uint8_t val  : 1;//当前按键的值
    uint32_t keep_time;
    uint32_t trigger_time;
}Key_TypeDef;

#define KEY_PORT    GPIOC
#define KEY_PIN     GPIO_PIN_13



extern Key_TypeDef key1;

void Key_Init(void);
void Key_Scan(void);

#endif

