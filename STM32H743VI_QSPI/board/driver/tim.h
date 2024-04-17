#ifndef __TIM_H
#define __TIM_H

extern TIM_HandleTypeDef TIM6_Handler, TIM7_Handler;

void TIM6_Init(void);
void TIM7_Init(void);

void TIM_DeInit(void);

#endif

